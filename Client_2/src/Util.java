import javafx.util.Pair;

import java.util.ArrayList;
import java.util.List;

public class Util {

    public List<List<Byte>> marshall(int request_id, int service_id, String input) {
        String[] arr = input.split( " ");
        return marshall( request_id,  service_id, arr);
    }

    public List<List<Byte>> marshall(int request_id, int service_id, String[] values) {

        // raw content: | service_id | content... |
        List<Byte> raw_content = new ArrayList<>();
        List<Pair<String, Integer>> params = Constants.get_request_params(service_id);

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

        // create message
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

}
