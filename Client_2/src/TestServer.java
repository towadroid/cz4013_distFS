import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.List;
import java.util.Map;

public class TestServer {

    public static void test() {
        DatagramSocket aSocket = null;
        int counter = 0;
        try{
            aSocket = new DatagramSocket(2302);
            //bound to host and port
            byte[] buffer = new byte[1000];
            while(true){
                DatagramPacket request
                        = new DatagramPacket(buffer, buffer.length);
                aSocket.receive(request); //blocked if no input
                System.out.println("Request received");
                if (counter > 0) {
                    DatagramPacket reply = new DatagramPacket(
                            request.getData(), request.getLength(),
                            request.getAddress(), request.getPort());
                    //to reply, send back to the same port
                    aSocket.send(reply);
                    System.out.println("Reply sent");
                }
                counter ++;
            }
        }
        //handle exceptions
        catch (Exception e) {
            System.out.println(e.getMessage());
        }
        finally {
            if (aSocket != null) aSocket.close();
        }
    }

    public static void main(String[] args) {
        test();
    }
}