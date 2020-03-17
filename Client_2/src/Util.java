import javafx.util.Pair;

import java.io.IOException;
import java.net.DatagramPacket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Util {

    public static List<List<Byte>> marshall(int request_id, int service_id, String input) {
        String[] arr = input.split( " ");
        return marshall( request_id,  service_id, arr);
    }

    /** For marshalling requests
     * @param request_id
     * @param service_id
     * @param values
     * @return
     */
    public static List<List<Byte>> marshall(int request_id, int service_id, String[] values) {
        List<Pair<String, Integer>> params = Constants.get_request_params(service_id);
        List<Byte> raw_content = marshall_to_content(service_id, params, values);
        List<List<Byte>> message = marshall_to_packets(request_id, raw_content);
        return message;
    }

    /** Send an entire message (which can contain many packets) to the server
     * @param message message to be sent
     * @throws IOException from sending packet
     */
    public static void send_message(List<List<Byte>> message, Runner runner) throws IOException{
        for (List<Byte> packet : message) {
            runner.send_packet(packet);
        }
    }

    /**Receive an entire message (which could contain many packets)
     * @return the content portion of the message
     * @throws IOException from socket receive
     */
    public List<Byte> receive_message(Runner runner) throws IOException{
        int total_packets = -1;
        List<Byte> all_content = new ArrayList<>();
        int current_packet = 0;
        while (total_packets == -1 || current_packet != total_packets) {
            byte[] packet = runner.receive_packet();
            int[] header = get_header(packet);
            int overall_content_size = header[1];
            if (total_packets == -1) {
                total_packets = (int) Math.ceil(overall_content_size*1.0/Constants.MAX_PACKET_CONTENT_SIZE);
            }
            all_content = add_byte_array(all_content, Arrays.copyOfRange(packet, Constants.PACKET_HEADER_SIZE, packet.length));
            current_packet++;
        }
        return all_content;
    }

    public static int[] get_header(byte[] packet) {
        int[] header = new int[3];
        header[0] = bytes_to_int(Arrays.copyOfRange(packet, 0, 4));
        header[1] = bytes_to_int(Arrays.copyOfRange(packet, 4, 8));
        header[2] = bytes_to_int(Arrays.copyOfRange(packet, 8, 12));
        return header;
    }

    // little-endian??
    public static int bytes_to_int(byte[] bytes) {
        return ((bytes[0] & 0xFF) << 0) |
                ((bytes[1] & 0xFF) << 8) |
                ((bytes[2] & 0xFF) << 16 ) |
                ((bytes[3] & 0xFF) << 24 );
    }

    public static List<Byte> marshall_to_content(int service_id, List<Pair<String, Integer>> params, String[] values) {
        // raw content: | service_id | content... |
        List<Byte> raw_content = new ArrayList<>();
        raw_content = add_int(service_id, raw_content);
        for (int i = 0; i < params.size(); i++) {
            int param_type = params.get(i).getValue();
            // strings are preceded by their length
            if (param_type == Constants.STRING_ID) {
                raw_content = add_int(values[i].length(), raw_content);
                raw_content = add_string(values[i], raw_content);
            }
            // ints NOT preceded by length
            else if (param_type == Constants.INT_ID) {
                raw_content = add_int(Integer.parseInt(values[i]), raw_content);
            }
        }
        return raw_content;
    }

    public static List<List<Byte>> marshall_to_packets(int request_id, List<Byte> raw_content) {
        int raw_content_size = raw_content.size();
        int total_packets = (int) Math.ceil(raw_content_size * 1.0 / Constants.MAX_PACKET_CONTENT_SIZE);
        List<List<Byte>> message = new ArrayList<>();
        for (int fragment = 0; fragment < total_packets; fragment++) {
            List<Byte> packet = new ArrayList<>();
            packet = add_int(request_id, packet);
            packet = add_int(raw_content_size, packet);
            packet = add_int(fragment, packet);
            int begin_index = fragment * Constants.MAX_PACKET_CONTENT_SIZE;
            int end_index;
            if (fragment == total_packets - 1) {
                end_index = raw_content_size;
            }
            else {
                end_index = (fragment+1) * Constants.MAX_PACKET_CONTENT_SIZE;
            }
            packet.addAll(raw_content.subList(begin_index, end_index));
        }
        return message;
    }

    public static byte[] to_primitive(List<Byte> in) {
        byte[] ret = new byte[in.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = in.get(i);
        }
        return ret;
    }

    public static String get_request_param_string(int service_id) {
        String ret = "Please enter: ";
        List<Pair<String, Integer>> params = Constants.get_request_params(service_id);
        for(Pair<String, Integer> p : params) {
            ret += (p.getKey() + " ");
        }
        return ret;
    }

    public static List<Byte> add_int(int num, List<Byte> in) {
        byte[] bytes = new byte[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = (byte) (num >>> (i*8));
        }
        return add_byte_array(in, bytes);
    }

    public static List<Byte> add_string(String str, List<Byte> in) {
        byte[] bytes = str.getBytes();
        return add_byte_array(in, bytes);
    }

    public static List<Byte> add_byte_array(List<Byte> in, byte[] add) {
        for (byte b : add) {
            in.add(b);
        }
        return in;
    }

    /** FOR DEBUGGING ONLY
     * @param request_id
     * @param service_id
     * @param values
     * @return
     */
    public static List<List<Byte>> marshall_reply(int request_id, int service_id, String[] values) {
        List<Pair<String, Integer>> params = Constants.get_successful_reply_params(service_id);
        List<Byte> raw_content = marshall_to_content(service_id, params, values);
        List<List<Byte>> message = marshall_to_packets(request_id, raw_content);
        return message;
    }

}
