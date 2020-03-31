import java.io.IOException;
import java.util.Map;

public class Read extends  Service {

    public Read(Runner r) {
        super(r);
        service_id = Constants.READ_REQUEST_ID;
    }

    @Override
    public void act() throws IOException {

        //ask for user input
        String request_param_string = get_request_param_string();
        System.out.println(request_param_string);
        String request_value_string = runner.scanner.nextLine();

        Map<String, Object> reply = send_and_receive(request_value_string);
        int file_content_size = ((String) reply.get("content")).length();
        System.out.println("reply received");

    }

}
