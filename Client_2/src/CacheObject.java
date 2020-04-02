import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class CacheObject {

    private String pathname;
    // the last time the file was edited at the server that we are aware of
    private int last_edit_time;
    // the last time we checked in with the server
    private long last_val_time;
    private HashMap<Integer, String> content;

    public CacheObject(String pn, Runner runner, int offset, int byte_count, String new_content) throws IOException {
        new CacheObject(pn, runner);
        set_cache(offset, byte_count, new_content);
    }

    public CacheObject(String pn, Runner runner) throws IOException {
        pathname = pn;
        // also sets last_val_time
        last_edit_time = check_server_edit_time(runner);
        content = new HashMap<>();
    }

    public boolean validate_local() {
        return System.currentTimeMillis() - last_val_time < Constants.FRESHNESS_INTERVAL;
    }

    public int check_server_edit_time(Runner runner) throws IOException {
        String[] request_values = {pathname};
        Map<String, Object> reply = Util.send_and_receive(Constants.EDIT_TIME_ID, request_values, runner);
        int server_edit_time = Integer.parseInt((String)reply.get("time"));
        last_val_time = System.currentTimeMillis();
        return server_edit_time;
    }

    public boolean validate_server(Runner runner) throws IOException {
        int server_edit_time = check_server_edit_time(runner);
        boolean fresh = true;
        if (last_edit_time != server_edit_time) {
            fresh = false;
            last_edit_time = server_edit_time;
            content = new HashMap<>();
        }
        return fresh;
    }

    public boolean has_range(int offset, int byte_count) {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        for (int i = start_block; i <= end_block; i++) {
            if (!content.containsKey(i)) {
                return false;
            }
        }
        return true;
    }

    public String get_cache(int offset, int byte_count) {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        String answer = "";
        for (int i = start_block; i <= end_block; i++) {
            answer += content.get(i);
        }
        return answer;
    }

    public void set_cache(int offset, int byte_count, String new_content) {
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        for (int i = start_block; i <= end_block; i++) {
            content.put(i,
                    new_content.substring(i*Constants.FILE_BLOCK_SIZE, (i+1)*Constants.FILE_BLOCK_SIZE));
        }
    }


    private int get_start_block(int offset) {
        return (int) Math.floor(offset * 1.0 / Constants.FILE_BLOCK_SIZE);
    }

    private int get_end_block(int offset, int byte_count) {
        return (int) Math.floor(offset+byte_count * 1.0 / Constants.FILE_BLOCK_SIZE);
    }
}
