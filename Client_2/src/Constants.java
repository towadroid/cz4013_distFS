import javafx.util.Pair;

import java.util.List;
import java.util.Map;

public class Constants {

    public static final String SERVER_NAME = "10.27.135.232";
    public static final int SERVER_PORT = 2302;

    public static final int INT_ID = 0;
    public static final int INT_SIZE = 4;
    public static final int STRING_ID = 1;
    public static final int MAX_PACKET_SIZE = 256;
    public static final int PACKET_HEADER_SIZE = 12;
    public static final int MAX_PACKET_CONTENT_SIZE = MAX_PACKET_SIZE - PACKET_HEADER_SIZE;
    public static final int TIMEOUT = 1000000;
    public static final boolean AT_MOST_ONCE = false;

    // Services
    public static final int SUCCESSFUL_SERVICE_ID = 0;
    public static final int EXIT_ID = 0;
    public static final int READ_REQUEST_ID = 1;
    public static final int WRITE_REQUEST_ID = 2;
    public static final int MONITOR_REQUEST_ID = 3;
    public static final int CLEAR_REQUEST_ID = 4;
    public static final int TRIM_REQUEST_ID = 5;
    public static final int EDIT_TIME_REQUEST_ID = 6;
    public static final int ACKNOWLEDGMENT_ID = 7;
    public static final int TEST_ID = 99;

    public static final String PROMPT = "Please enter: 0 for exit, 1 for read, 2 for write, 3 for monitor, " +
            "4 for clear, 5 for trim, 99 for debug";

    public static final List<Pair<String, Integer>> READ_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID), new Pair<>("offset", INT_ID), new Pair<>("byte_count", INT_ID));
    public static final List<Pair<String, Integer>> READ_REPLY_PARAMS =
            List.of(new Pair<>("content", STRING_ID));
    public static final List<Pair<String, Integer>> WRITE_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID), new Pair<>("offset", INT_ID), new Pair<>("content", STRING_ID));
    public static final List<Pair<String, Integer>> WRITE_REPLY_PARAMS =
            List.of();
    public static final List<Pair<String, Integer>> MONITOR_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID), new Pair<>("monitor_time", INT_ID));
    public static final List<Pair<String, Integer>> MONITOR_REPLY_PARAMS =
            List.of();
    public static final List<Pair<String, Integer>> CLEAR_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID));
    public static final List<Pair<String, Integer>> CLEAR_REPLY_PARAMS =
            List.of();
    public static final List<Pair<String, Integer>> TRIM_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID));
    public static final List<Pair<String, Integer>> TRIM_REPLY_PARAMS =
            List.of();
    public static final List<Pair<String, Integer>> EDIT_TIME_REQUEST_PARAMS =
            List.of(new Pair<>("pathname", STRING_ID));
    public static final List<Pair<String, Integer>> EDIT_TIME_REPLY_PARAMS =
            List.of(new Pair<>("time", INT_ID));
    public static final List<Pair<String, Integer>> ACKNOWLEDGE_REQUEST_PARAMS =
            List.of();

    public static final Map<Integer, List<Pair<String, Integer>>> REQUEST_PARAMS;
    static{
        REQUEST_PARAMS = Map.of(
                READ_REQUEST_ID, READ_REQUEST_PARAMS,
                WRITE_REQUEST_ID, WRITE_REQUEST_PARAMS,
                MONITOR_REQUEST_ID, MONITOR_REQUEST_PARAMS,
                CLEAR_REQUEST_ID, CLEAR_REQUEST_PARAMS,
                TRIM_REQUEST_ID, TRIM_REQUEST_PARAMS,
                EDIT_TIME_REQUEST_ID, EDIT_TIME_REQUEST_PARAMS,
                ACKNOWLEDGMENT_ID, ACKNOWLEDGE_REQUEST_PARAMS);
    }

    public static final Map<Integer, List<Pair<String, Integer>>> SUCCESSFUL_REPLY_PARAMS;
    static{
        SUCCESSFUL_REPLY_PARAMS = Map.of(
                READ_REQUEST_ID, READ_REPLY_PARAMS,
                WRITE_REQUEST_ID, WRITE_REPLY_PARAMS,
                MONITOR_REQUEST_ID, MONITOR_REPLY_PARAMS,
                CLEAR_REQUEST_ID, CLEAR_REPLY_PARAMS,
                TRIM_REQUEST_ID, TRIM_REPLY_PARAMS,
                EDIT_TIME_REQUEST_ID, EDIT_TIME_REPLY_PARAMS);
    }

    public static final Map<Integer, String> FAILED_REPLY_MESSAGES;
    static{
        FAILED_REPLY_MESSAGES = Map.of(1, "file doesn't exit",
                2, "bad range parameters",
                3, "file already empty",
                4, "file was changed");
    }

    public static List<Pair<String, Integer>> get_request_params(int service_id) {
        return REQUEST_PARAMS.get(service_id);
    }
    public static List<Pair<String, Integer>> get_successful_reply_params(int service_id) {
        return SUCCESSFUL_REPLY_PARAMS.get(service_id);
    }
    public static String get_failed_reply_params(int status_id) {
        return FAILED_REPLY_MESSAGES.get(status_id);
    }
}
