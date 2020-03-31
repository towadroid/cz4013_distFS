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

    public abstract void act() throws IOException;

    public static Service generate_service(int service_id, Runner r) {
        if (service_id == Constants.READ_REQUEST_ID) {
            return new Read(r);
        }
        else if (service_id == Constants.WRITE_REQUEST_ID) {
            return new Write(r);
        }
        else if (service_id == Constants.TEST_ID) {
            return new TestService(r);
        }
        else {
            return null;
        }
    }

    public Map<String, Object> send_and_receive(String[] input) throws IOException {
        return send_and_receive(service_id, input);
    }

    public Map<String, Object> send_and_receive(int service_id, String[] input) throws IOException {
        runner.socket.setSoTimeout(Constants.TIMEOUT);
        List<Byte> reply_content;
        List<List<Byte>> request = Util.marshall(runner.get_request_id(), service_id, input);
        Util.send_message(request, runner);
        System.out.println("Request sent");
        while(true) {
            try {
                reply_content = Util.receive_message(runner);
                System.out.println("Reply received");
                break;
            }
            catch (SocketTimeoutException t) {
                System.out.println("Request re-sent");
                Util.send_message(request, runner);
            }
        }
        Map<String, Object> reply = Util.un_marshall(service_id, reply_content);

        if (Constants.AT_MOST_ONCE) {
            // upon receiving, send acknowledgment
            List<List<Byte>> ack = Util.marshall(runner.get_request_id(), Constants.ACKNOWLEDGMENT_ID, new String[0]);
            Util.send_message(ack, runner);
        }

        runner.increment_request_id();
        return reply;
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
