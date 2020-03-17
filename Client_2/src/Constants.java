import javafx.util.Pair;

import java.net.InetAddress;
import java.util.List;

public class Constants {

    public static final String SERVER_NAME = "localhost";
    public static final int SERVER_PORT = 6789;

    public static final int INT_ID = 0;
    public static final int STRING_ID = 1;
    public static final int MAX_PACKET_SIZE = 256;
    public static final int PACKET_HEADER_SIZE = 12;
    public static final int MAX_PACKET_CONTENT_SIZE = MAX_PACKET_SIZE - PACKET_HEADER_SIZE;

    // Services
    public static final int EXIT_ID = 0;
    public static final int MONITOR_REQUEST_ID = 3;
    public static final String PROMPT = "Please enter: 0 for exit, 1 for read, 2 for write, 3 for monitior";

    public static final String[] test = {"a"};

    public static final int READ_REQUEST_ID = 1;
    public static final List<Pair<String, Integer>> READ_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID), new Pair<>("offset", INT_ID), new Pair<>("byte_count", INT_ID));
    public static final List<Pair<String, Integer>> READ_REPLY_PARAMS =
            List.of(new Pair<>("bytes_read", INT_ID), new Pair<>("content", STRING_ID));

    public static final int WRITE_REQUEST_ID = 2;
    public static final List<Pair<String, Integer>> WRITE_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID), new Pair<>("offset", INT_ID), new Pair<>("content", STRING_ID));
    public static final List<Pair<String, Integer>> WRITE_REPLY_PARAMS =
            List.of();

    // TODO: implement this
    public static List<Pair<String, Integer>> get_request_params(int service_id) {
        return READ_REPLY_PARAMS;
    }

}
