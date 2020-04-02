import java.io.IOException;
import java.net.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class Runner {

    public Scanner scanner;
    public InetAddress host;
    public DatagramSocket socket;
    public HashMap<String, CacheObject> cache;
    private int request_id = 0;
    private int server_port;

    public Runner(String s_name, int s_port) throws UnknownHostException, SocketException {
        socket = new DatagramSocket();
        scanner = new Scanner(System.in);
        server_port = s_port;
        host = InetAddress.getByName(s_name);
        cache = new HashMap<>();
    }

    /**Send one packet to the server
     * @param packet_list packet to be sent
     * @throws IOException from sending packet
     */
    public void send_packet(List<Byte> packet_list) throws IOException{
        byte[] packet = Util.to_primitive(packet_list);
        DatagramPacket request = new DatagramPacket(packet,
                packet.length, host, server_port);
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

    public void increment_request_id() {
        request_id ++;
    }
    public int get_request_id() {
        return request_id;
    }

    public static void main(String[] args) throws IOException {
        // Server name: 10.27.135.232
        // Server port: 2302
        Runner runner = new Runner(args[0], Integer.parseInt(args[1]));
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
