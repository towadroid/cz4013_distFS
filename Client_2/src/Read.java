import java.io.IOException;
import java.util.Map;

public class Read extends  Service {

    public Read(Runner r) {
        super(r);
    }

    @Override
    public void act() throws IOException {

        //ask for user input
        String request_param_string = Util.get_request_param_string(Constants.READ_REQUEST_ID);
        System.out.println(request_param_string);
        String request_value_string = runner.scanner.nextLine();

        Map<String, Object> reply = Util.send_and_receive(runner.request_id, Constants.READ_REQUEST_ID,
                request_value_string, runner);
        int file_content_size = ((String) reply.get("content")).length();
        System.out.println("reply received");

    }

}
