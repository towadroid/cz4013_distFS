import java.io.IOException;
import java.net.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Runner {

    Scanner scanner;
    InetAddress host;
    DatagramSocket socket;

    public Runner() throws UnknownHostException, SocketException {
        host = InetAddress.getByName(Constants.SERVER_NAME);
        socket = new DatagramSocket();
        scanner = new Scanner(System.in);
    }

    /**Send one packet to the server
     * @param packet_list packet to be sent
     * @throws IOException from sending packet
     */
    public void send_packet(List<Byte> packet_list) throws IOException{
        byte[] packet = Util.to_primitive(packet_list);
        DatagramPacket request = new DatagramPacket(packet,
                packet.length, host, Constants.SERVER_PORT);
        socket.send(request);
    }

    /**Receive one packet from the server
     * @return byte array of the entire packet
     * @throws IOException from receiving packet
     */
    public byte[] receive_packet() throws IOException {
        byte[] buffer = new byte[Constants.MAX_PACKET_SIZE];
        DatagramPacket reply = new DatagramPacket(buffer, buffer.length);
        socket.receive(reply);
        return reply.getData();
    }

    public static void main(String[] args) throws SocketException, UnknownHostException {
        Runner runner = new Runner();
        while (true) {
            System.out.println(Constants.PROMPT);
            int input = runner.scanner.nextInt();
            runner.scanner.nextLine();
            if (input == Constants.EXIT_ID) {
                break;
            }
            Service requested_service = Service.generate_service(input, runner);
            if (!(requested_service==null)) {
                requested_service.act();
            }
            System.out.println("");
        }
        runner.socket.close();
    }

}
