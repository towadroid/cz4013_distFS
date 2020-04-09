package Helpers;

import Services.Service;

import java.io.IOException;
import java.net.*;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

/**
 * Holds server connection info, as well as anything that remains across multiple calls to services
 */
public class Runner {

    public Scanner scanner;
    public InetAddress host;
    public DatagramSocket socket;
    public HashMap<String, CacheObject> cache;
    private int request_id = 0;
    private int server_port;

    /**
     * @param s_name server name
     * @param s_port server port
     * @throws UnknownHostException
     * @throws SocketException
     */
    public Runner(String s_name, int s_port) throws UnknownHostException, SocketException {
        socket = new DatagramSocket();
        scanner = new Scanner(System.in);
        server_port = s_port;
        host = InetAddress.getByName(s_name);
        cache = new HashMap<>();
    }

    /**Send one packet to the server
     * Simulate packets being lost in transmission
     * @param packet_list packet to be sent
     * @throws IOException from sending packet
     */
    public void send_packet(List<Byte> packet_list) throws IOException{
        byte[] packet = Util.to_primitive(packet_list);
        DatagramPacket request = new DatagramPacket(packet,
                packet.length, host, server_port);
        double random = Math.random();
        if (random >= Constants.NETWORK_FAILURE_RATE) {
            socket.send(request);
        }
        else if (Constants.DEBUG) {
            System.out.println("Simulating send failure");
        }
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

}
