import java.io.IOException;
import java.util.Map;

public class TestService extends Service {

    public TestService(Runner r) {
        super(r);
        service_id = Constants.WRITE_REQUEST_ID;
    }

    @Override
    public void act() throws IOException {
        //ask for user input
        String[] request_value_string = get_user_request_values();

        Map<String, Object> reply = send_and_receive(request_value_string);

        if ((int) reply.get("status") == 0) {
            System.out.println("request success");
        }
        else {
            System.out.println("error: " + reply.get("message"));
        }
    }

}
