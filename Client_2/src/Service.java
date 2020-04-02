import javafx.util.Pair;

import java.io.IOException;
import java.net.SocketTimeoutException;
import java.util.List;
import java.util.Map;

public abstract class Service {

    public Runner runner;
    public int service_id;

    public Service(Runner r) {
        runner = r;
    }

    public void act() throws IOException {
        //ask for user input
        String[] request_values = get_user_request_values();
        Map<String, Object> reply = send_and_receive(request_values);
        if ((int) reply.get("status") == 0) {
            System.out.println("request success");
        }
        else {
            System.out.println("error: " + reply.get("message"));
        }
    }

    public static Service generate_service(int service_id, Runner r) {
        if (service_id == Constants.READ_ID) {
            return new Read(r);
        }
        else if (service_id == Constants.WRITE_ID) {
            return new Write(r);
        }
        else if (service_id == Constants.MONITOR_ID) {
            return new Monitor(r);
        }
        else if (service_id == Constants.CLEAR_ID) {
            return new Clear(r);
        }
        else if (service_id == Constants.TRIM_ID) {
            return new Trim(r);
        }
        else if (service_id == Constants.TEST_ID) {
            return new TestService(r);
        }
        else {
            return null;
        }
    }

    public Map<String, Object> send_and_receive(String[] input) throws IOException {
        return Util.send_and_receive(service_id, input, runner);
    }

    public String[] get_user_request_values() {
        List<Pair<String, Integer>> params = Constants.get_request_params(service_id);
        String[] ret = new String[params.size()];
        for(int i = 0; i < params.size(); i++) {
            Pair<String, Integer> p = params.get(i);
            String prompt =  "Please enter the " + p.getKey() + ": ";
            System.out.println(prompt);
            ret[i] = runner.scanner.nextLine();
        }
        return ret;
    }




}
