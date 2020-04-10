package Helpers;

import Exceptions.ApplicationException;
import Exceptions.BadRangeException;
import Exceptions.BadPathnameException;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * Holds information for storing read content in the cache
 */
public class CacheObject {

    private String pathname;
    // the last time we checked in with the server
    private long server_checkin_time;
    // the last time the file was edited at the server that we are aware of
    private int last_known_edit_time;

    private HashMap<Integer, String> content;
    private int final_block;

    /** Set the pathname, server_checkin_time, and last_known_edit time
     * @param pn pathname
     * @param runner connection info
     * @throws IOException sending/receiving message
     * @throws BadPathnameException if requesting the edit time at the server yields bad pathname
     */
    public CacheObject(String pn, Runner runner) throws IOException, BadPathnameException {
        pathname = pn;
        // also sets server_checkin_time
        last_known_edit_time = get_server_edit_time(runner);
        content = new HashMap<>();
        final_block = -1;
    }

    /** Grab the requested string from the cache, assuming the necessary blocks exist
     * @param offset file offset
     * @param byte_count number of bytes to read
     * @return requested string from cache
     * @throws BadRangeException see check_range method (bottom)
     */
    public String get_cache(int offset, int byte_count) throws BadRangeException {
        check_range(offset, byte_count);
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        int start_index = offset%Constants.FILE_BLOCK_SIZE;
        int end_index = (offset+byte_count)%Constants.FILE_BLOCK_SIZE;
        if (start_block == end_block) {
            return content.get(start_block).substring(start_index, end_index);
        }
        StringBuilder answer = new StringBuilder();
        answer.append(content.get(start_block).substring(start_index));
        for (int i = start_block+1; i < end_block; i++) {
            answer.append(content.get(i));
        }
        answer.append(content.get(end_block).substring(0,end_index));
        return answer.toString();
    }

    /** Set the cache with the content read by the server
     * Set entire blocks, not just the range requested
     * @param offset file offset
     * @param byte_count number of bytes read
     * @param new_content read content returned by the server (in blocks)
     * @throws BadRangeException see check_range method (bottom)
     */
    public void set_cache(int offset, int byte_count, String new_content) throws BadRangeException {
        check_range(offset, byte_count);
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        for (int i = start_block; i < end_block; i++) {
            content.put(i,
                    new_content.substring(i*Constants.FILE_BLOCK_SIZE, (i+1)*Constants.FILE_BLOCK_SIZE));
        }
        String last_piece = new_content.substring(end_block*Constants.FILE_BLOCK_SIZE);
        if (last_piece.length() != Constants.FILE_BLOCK_SIZE) {
            System.out.println("final block set to " + end_block);
            final_block = end_block;
        }
        content.put(end_block, last_piece);
    }

    /** Whether we must read the content from the server
     * There are two cases where we need to read from the server:
     * 1. if it is not cached locally
     * 2. if it is cached locally, expired the freshness interval, and has been edited at the server
     * @param offset file offset
     * @param byte_count number of bytes to read
     * @param runner connection info
     * @return whether or not one must read the server
     * @throws IOException send/receive message
     * @throws BadPathnameException if requesting edit time at server yields bad pathname
     * @throws BadRangeException if the given offset/byte_count combo is certain to be out of range
     */
    public boolean must_read_server(int offset, int byte_count, Runner runner) throws IOException, BadPathnameException, BadRangeException {
        boolean must = !cached(offset, byte_count) || (!local_fresh() && !server_fresh(runner));
        if (Constants.DEBUG) {
            if (must) {
                System.out.println("Must read from server");
            }
            else {
                System.out.println("No need to read from server");
            }
        }
        return must;
    }

    /** Whether the requested offset/byte_count combo already exist in the cache
     * @param offset file offset
     * @param byte_count number of bytes to read
     * @return whether it exists in the cache
     * @throws BadRangeException if the given offset/byte_count combo is certain to be out of range
     */
    private boolean cached(int offset, int byte_count) throws BadRangeException {
        check_range(offset, byte_count);
        int start_block = get_start_block(offset);
        int end_block = get_end_block(offset, byte_count);
        for (int i = start_block; i <= end_block; i++) {
            if (!content.containsKey(i)) {
                if (Constants.DEBUG) System.out.println("Checking cache: NOT cached");
                return false;
            }
        }
        if (Constants.DEBUG) System.out.println("Checking cache: cached");
        return true;
    }

    /** Whether the freshness interval has expired or not
     * @return expired?
     */
    private boolean local_fresh() {
        boolean fresh = System.currentTimeMillis() - server_checkin_time < Constants.FRESHNESS_INTERVAL;
        if (Constants.DEBUG) {
            if (fresh) {
                System.out.println("Checking freshness interval: fresh");
            }
            else {
                System.out.println("Checking freshness interval: NOT fresh");
            }
        }
        return fresh;
    }

    /** Whether the last update time at the server matches our last known edit time
     * If it doesn't match, then we must clear the cache (as it is now out of date)
     * @param runner connection info
     * @return match?
     * @throws IOException send/receive messages
     * @throws BadPathnameException if requesting the edit time at the server yields bad pathname
     */
    private boolean server_fresh(Runner runner) throws IOException, BadPathnameException {
        int last_edit_time = get_server_edit_time(runner);
        if (last_known_edit_time == last_edit_time) {
            if (Constants.DEBUG) System.out.println("Checking server: NO new updates");
            return true;
        }
        else{
            if (Constants.DEBUG) System.out.println("Checking server: new updates. Clearing cache");
            last_known_edit_time = last_edit_time;
            content = new HashMap<>();
            final_block = -1;
            return false;
        }
    }

    /** Get the time of the server's last update
     * automatically update server_checkin_time
     * @param runner connection info
     * @return the time
     * @throws IOException send/receive messages
     * @throws BadPathnameException if requesting the edit time at the server yields bad pathname
     */
    private int get_server_edit_time(Runner runner) throws IOException, BadPathnameException {
        server_checkin_time = System.currentTimeMillis();

        String[] request_values = {pathname};
        try {
            Map<String, Object> reply = Util.send_and_receive(Constants.EDIT_TIME_ID, request_values, runner);
            return (int) reply.get("time");
        }
        catch (BadPathnameException nsfe) {
            throw new BadPathnameException();
        }
        // this should never happen
        catch (ApplicationException ae) {
            System.out.println("unexpected error: " + ae.getMessage());
            return -1;
        }
    }

    private int get_start_block(int offset) {
        return (int) Math.floor(offset * 1.0 / Constants.FILE_BLOCK_SIZE);
    }

    private int get_end_block(int offset, int byte_count) {
        return (int) Math.floor((offset+byte_count) * 1.0 / Constants.FILE_BLOCK_SIZE);
    }

    /** Check if the given offset/byte_count combo is certain to be out of range
     * @param offset file offset
     * @param byte_count number of bytes to read
     * @throws BadRangeException if the combo is indeed out of range
     */
    // TODO: fix this
    private void check_range(int offset, int byte_count) throws BadRangeException {
        int end_block = get_end_block(offset, byte_count);
        if (offset < 0 ||
            byte_count < 0 ||
            (final_block != -1 && final_block == end_block &&
                (offset+byte_count) % Constants.FILE_BLOCK_SIZE > content.get(end_block).length()) ||
            (final_block != -1 && final_block > end_block))
        {
            throw new BadRangeException();
        }
    }
}


