import java.io.IOException;
import java.net.SocketTimeoutException;
import java.util.List;
import java.util.Map;

public class Monitor extends Service {

    public Monitor(Runner r) {
        super(r);
        service_id = Constants.MONITOR_ID;
    }

    @Override
    public void act() throws IOException {

        // ask for user input
        String[] request_values = get_user_request_values();
        // in milliseconds
        int monitor_period = Integer.parseInt(request_values[1]);
        long monitor_start = System.currentTimeMillis();
        int monitor_request_id = runner.get_request_id();

        Map<String, Object> monitor_reply = send_and_receive(request_values);
        List<Byte> update_bytes;

        if ((int) monitor_reply.get("status") == Constants.SUCCESSFUL_STATUS_ID) {
            System.out.println("request success");
            runner.socket.setSoTimeout(monitor_period);
            try {
                while(true) {
                    long current_time = System.currentTimeMillis();
                    if (current_time - monitor_start >= monitor_period) {
                        throw new SocketTimeoutException();
                    }
                    runner.socket.setSoTimeout((int) (monitor_period - (current_time - monitor_start)));
                    try {
                        // updates must have same request id as the original monitor request
                        update_bytes = Util.receive_message(monitor_request_id, runner);
                        // we know that service id is not needed here
                        Map<String, Object> update = Util.un_marshall(-1, update_bytes);
                        System.out.println("Update received: " + update.get("content"));
                    }
                    catch (CorruptMessageException c) {
                        System.out.println("Received corrupt message; Throwing away");
                    }
                }
            }
            catch (SocketTimeoutException t) {
                System.out.println("Timed out; Done receiving updates");
            }
        }
        else {
            System.out.println("error: " + monitor_reply.get("message"));
        }
    }

}
