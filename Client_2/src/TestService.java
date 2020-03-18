import java.io.IOException;
import java.util.Map;

public class TestService extends Service {

    public TestService(Runner r) {
        super(r);
    }

    @Override
    public void act() throws IOException {
        //ask for user input
        String request_param_string = Util.get_request_param_string(Constants.WRITE_REQUEST_ID);
        System.out.println(request_param_string);
        String request_value_string = runner.scanner.nextLine();

        Map<String, Object> reply = Util.send_and_receive(runner.request_id, Constants.WRITE_REQUEST_ID,
                request_value_string, runner);

        if ((int) reply.get("status") == 0) {
            System.out.println("request success");
        }
        else {
            System.out.println("error: " + reply.get("message"));
        }
    }

}
