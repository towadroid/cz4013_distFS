package Helpers;

import Exceptions.CorruptMessageException;

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

    private int server_port;
    private int request_id = 0;

    public int freshness_interval;
    public boolean at_most_once;
    public double network_failure_rate;

    /**
     * @param s_name server name
     * @param s_port server port
     * @throws UnknownHostException
     * @throws SocketException
     */
    public Runner(Scanner s, String s_name, int s_port, int amo, double nfr, int f_interval)
            throws UnknownHostException, SocketException {
        socket = new DatagramSocket();
        scanner = s;
        server_port = s_port;
        host = InetAddress.getByName(s_name);
        cache = new HashMap<>();
        freshness_interval = f_interval;
        at_most_once = amo == 1;
        network_failure_rate = nfr;
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
        if (random >= network_failure_rate) {
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

    /** acknowledge any lingering old replies (if we are using at most once semantics)
     * @throws IOException from receiving packet
     */
    public void close() throws IOException {
        if (at_most_once) {
            socket.setSoTimeout(Constants.TIMEOUT);
            if (Constants.DEBUG) System.out.println("(log) Begin acknowledging old replies");
            while(true) {
                try {
                    Util.receive_message(request_id, this);
                }
                catch (SocketTimeoutException t) {
                    if (Constants.DEBUG) System.out.println("(log) Socket timeout; Done with cleanup");
                    break;
                }
                catch (CorruptMessageException c) {
                    if (Constants.DEBUG) System.out.println("(log) Throwing away corrupt message");
                }
            }
        }
        socket.close();
    }

    public void increment_request_id() {
        request_id ++;
    }
    public int get_request_id() {
        return request_id;
    }

}
