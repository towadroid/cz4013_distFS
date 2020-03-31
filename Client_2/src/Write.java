import java.io.IOException;
import java.util.Map;

public class Write extends Service {

    public Write(Runner r) {
        super(r);
        service_id = Constants.WRITE_REQUEST_ID;
    }

    @Override
    public void act() throws IOException {

        //ask for user input
        String request_param_string = get_request_param_string();
        System.out.println(request_param_string);
        String request_value_string = runner.scanner.nextLine();

        Map<String, Object> reply = send_and_receive(request_value_string);

        if ((int) reply.get("status") == 0) {
            System.out.println("request success");
        }
        else {
            System.out.println("error: " + reply.get("message"));
        }
    }

}
