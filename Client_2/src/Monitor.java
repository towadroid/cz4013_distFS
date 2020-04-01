import java.io.IOException;
import java.util.Map;

public class Monitor extends Service {

    public Monitor(Runner r) {
        super(r);
        service_id = Constants.MONITOR_REQUEST_ID;
    }

    @Override
    public void act() throws IOException {

        // ask for user input
        String[] request_values = get_user_request_values();
        // in milliseconds
        int monitor_time = Integer.parseInt(request_values[1]);

        Map<String, Object> reply = send_and_receive(request_values);

        if ((int) reply.get("status") == 0) {
            System.out.println("request success");



        }
        else {
            System.out.println("error: " + reply.get("message"));
        }
    }

}
