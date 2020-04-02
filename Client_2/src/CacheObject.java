import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class CacheObject {

    private String pathname;
    // the last time the file was edited at the server that we are aware of
    private int last_known_edit_time;
    // the last time we checked in with the server
    private long server_checkin_time;
    private HashMap<Integer, String> content;
    private int final_block = -1;

    public CacheObject(String pn, Runner runner) throws IOException, ApplicationException {
        pathname = pn;
        // also sets server_checkin_time
        last_known_edit_time = get_server_edit_time(runner);
        content = new HashMap<>();
    }

    public String get_cache(int offset, int byte_count) {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        StringBuilder answer = new StringBuilder();
        for (int i = start_block; i <= end_block; i++) {
            answer.append(content.get(i));
        }
        return answer.toString();
    }

    public void set_cache(int offset, int byte_count, String new_content) {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        for (int i = start_block; i < end_block; i++) {
            content.put(i,
                    new_content.substring(i*Constants.FILE_BLOCK_SIZE, (i+1)*Constants.FILE_BLOCK_SIZE));
        }
        String last_piece = new_content.substring(end_block*Constants.FILE_BLOCK_SIZE);
        if (last_piece.length() != Constants.FILE_BLOCK_SIZE) {
            final_block = end_block;
        }
        content.put(end_block, last_piece);
    }

    public boolean must_read_server(int offset, int byte_count, Runner runner) throws IOException, ApplicationException {
        return !cached(offset, byte_count) || (!local_fresh() && !server_fresh(runner));
    }

    private boolean cached(int offset, int byte_count) throws ApplicationException {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        if (offset < 0 || end_block > final_block) {
            throw new ApplicationException(Constants.ERROR_MESSAGES.get(Constants.BAD_RANGE_ID));
        }

        for (int i = start_block; i <= end_block; i++) {
            if (!content.containsKey(i)) {
                return false;
            }
        }
        return true;
    }

    private boolean local_fresh() {
        return System.currentTimeMillis() - server_checkin_time < Constants.FRESHNESS_INTERVAL;
    }

    private boolean server_fresh(Runner runner) throws IOException, ApplicationException {
        int last_edit_time = get_server_edit_time(runner);
        if (last_known_edit_time == last_edit_time) {
            return true;
        }
        else{
            last_known_edit_time = last_edit_time;
            content = new HashMap<>();
            final_block = -1;
            return false;
        }
    }

    private int get_server_edit_time(Runner runner) throws IOException, ApplicationException {
        server_checkin_time = System.currentTimeMillis();

        String[] request_values = {pathname};
        Map<String, Object> reply = Util.send_and_receive(Constants.EDIT_TIME_ID, request_values, runner);
        return Integer.parseInt((String)reply.get("time"));
    }

    private int get_start_block(int offset) {
        return (int) Math.floor(offset * 1.0 / Constants.FILE_BLOCK_SIZE);
    }

    private int get_end_block(int offset, int byte_count) {
        return (int) Math.floor(offset+byte_count * 1.0 / Constants.FILE_BLOCK_SIZE);
    }
}
