import java.io.IOException;
import java.util.Map;

public class Read extends  Service {

    public Read(Runner r) {
        super(r);
        service_id = Constants.READ_ID;
    }

    @Override
    public void act() throws IOException {

        //ask for user input
        String[] request_values = get_user_request_values();

        Map<String, Object> reply = send_and_receive(request_values);
        int file_content_size = ((String) reply.get("content")).length();
        System.out.println("reply received");

    }

}
