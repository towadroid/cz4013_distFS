import javafx.util.Pair;

import java.io.IOException;
import java.net.SocketTimeoutException;
import java.util.*;

public class Util {

    public static Map<String, Object> send_and_receive(int service_id, String[] input, Runner runner) throws IOException, ApplicationException {
        runner.socket.setSoTimeout(Constants.TIMEOUT);
        List<Byte> reply_content;
        List<List<Byte>> request = Util.marshall(runner.get_request_id(), service_id, input);
        Util.send_message(request, runner);
        System.out.println("Request sent");
        while(true) {
            try {
                reply_content = Util.receive_message(runner.get_request_id(), runner);
                System.out.println("Reply received");
                break;
            }
            catch (SocketTimeoutException t) {
                System.out.println("Timed out; Request re-sent");
                Util.send_message(request, runner);
            }
            catch (CorruptMessageException c) {
                // may want to wait a bit here?
                System.out.println("Received corrupt message; Throwing away");
            }
        }
        Map<String, Object> reply = Util.un_marshall(service_id, reply_content);

        if (Constants.AT_MOST_ONCE) {
            // upon receiving, send acknowledgment
            List<List<Byte>> ack = Util.marshall(runner.get_request_id(), Constants.ACKNOWLEDGMENT_ID, new String[0]);
            Util.send_message(ack, runner);
        }

        runner.increment_request_id();
        return reply;
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
     * @param check_request_id check received request ids against this one
     * @param runner
     * @return the content portion of the message
     * @throws IOException from socket receive
     */

    public static List<Byte> receive_message(int check_request_id, Runner runner) throws IOException {
        int total_packets = -1;
        List<Byte> all_content = new ArrayList<>();
        int current_packet = 0;
        int overall_content_size = -1;
        while (total_packets == -1 || current_packet != total_packets) {
            byte[] packet = runner.receive_packet();
            int[] header = get_header(packet);
            int receive_request_id = header[0];
            overall_content_size = header[1];
            int fragment_number = header[2];
            if (fragment_number != current_packet || receive_request_id != check_request_id) {
                throw new CorruptMessageException();
            }
            if (total_packets == -1) {
                total_packets = (int) Math.ceil(overall_content_size*1.0/Constants.MAX_PACKET_CONTENT_SIZE);
            }
            add_byte_array(all_content, Arrays.copyOfRange(packet, Constants.PACKET_HEADER_SIZE, packet.length));
            current_packet++;
        }
        all_content = all_content.subList(0, overall_content_size);
        return all_content;
    }

    public static Map<String, Object> un_marshall(int service_id, List<Byte> raw_content) throws ApplicationException {
        Map<String, Object> message = new HashMap<>();
        int status_id = bytes_to_int(raw_content.subList(0, 4));
        message.put("status_id", status_id);
        List<Pair<String, Integer>> params;

        // if the status is "successful", then the parameters of the message depend on the SERVICE ID given
        // i.e. a successful read vs. successful write vs. successful monitor have different parameters
        if (status_id == Constants.SUCCESSFUL_STATUS_ID) {
            params = Constants.get_successful_reply_params(service_id);
        }

        // if the status is not "successful", then the parameters of the message depend on the STATUS ID given
        // i.e. one of the error messages or an update notification
        // a status that is not "successful" is called an "alert"
        else {
            // if its an error message, then get the correct error message from Constants
            if (Constants.ERROR_MESSAGES.containsKey(status_id)) {
                String fail_message = Constants.ERROR_MESSAGES.get(status_id);
                throw new ApplicationException(fail_message);
            }
            params = Constants.get_alert_reply_params(status_id);
        }

        // match raw_content with params
        int counter = 4;
        for (Pair<String, Integer> param : params) {
            int param_type = param.getValue();
            int i = bytes_to_int(raw_content.subList(counter, counter + Constants.INT_SIZE));
            counter += Constants.INT_SIZE;
            message.put(param.getKey(), i);
            if (param_type == Constants.STRING_ID) {
                String s = new String(to_primitive(raw_content.subList(counter, counter + i)));
                counter += i;
                message.put(param.getKey(), s);
            }
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

    private static int[] get_header(byte[] packet) {
        int[] header = new int[3];
        header[0] = bytes_to_int(Arrays.copyOfRange(packet, 0, 4));
        header[1] = bytes_to_int(Arrays.copyOfRange(packet, 4, 8));
        header[2] = bytes_to_int(Arrays.copyOfRange(packet, 8, 12));
        return header;
    }

    // little-endian??
    private static int bytes_to_int(byte[] bytes) {
        return ((bytes[3] & 0xFF) << 0) |
                ((bytes[2] & 0xFF) << 8) |
                ((bytes[1] & 0xFF) << 16 ) |
                ((bytes[0] & 0xFF) << 24 );
    }

    // little-endian??
    private static int bytes_to_int(List<Byte> bytes) {
        return ((bytes.get(3) & 0xFF) << 0) |
                ((bytes.get(2) & 0xFF) << 8) |
                ((bytes.get(1) & 0xFF) << 16 ) |
                ((bytes.get(0) & 0xFF) << 24 );
    }

    private static List<Byte> marshall_to_content(int service_id, List<Pair<String, Integer>> params, String[] values) {
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

    private static List<List<Byte>> marshall_to_packets(int request_id, List<Byte> raw_content) {
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
            message.add(packet);
        }
        return message;
    }

    // big-endian
    private static List<Byte> add_int(int num, List<Byte> in) {
        byte[] bytes = new byte[4];
        for (int i = 0; i < 4; i++) {
            bytes[4-i-1] = (byte) (num >>> (i*8));
        }
        return add_byte_array(in, bytes);
    }

    private static List<Byte> add_string(String str, List<Byte> in) {
        byte[] bytes = str.getBytes();
        return add_byte_array(in, bytes);
    }

    private static List<Byte> add_byte_array(List<Byte> in, byte[] add) {
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
    private static List<List<Byte>> marshall_reply(int request_id, int service_id, String[] values) {
        List<Pair<String, Integer>> params = Constants.get_successful_reply_params(service_id);
        List<Byte> raw_content = marshall_to_content(service_id, params, values);
        List<List<Byte>> message = marshall_to_packets(request_id, raw_content);
        return message;
    }

}
