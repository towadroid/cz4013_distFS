package Services;

import Exceptions.ApplicationException;
import Helpers.Constants;
import Helpers.Runner;
import Helpers.Util;
import javafx.util.Pair;

import java.io.IOException;
import java.util.List;
import java.util.Map;

/**
 * Abstract class for services that this application can perform for the client
 * Sub-classes: Read, Write, Monitor, Clear, Trim
 */
public abstract class Service {

    public Runner runner;
    public int service_id;

    public Service(Runner r) {
        runner = r;
    }

    /** Perform the service
     * This basic method (shared by Write, Trim, and Clear) can be overridden for more involved services (Monitor, Write)
     * @throws IOException send/receive message
     */
    public void act() throws IOException {
        //ask for user input
        String[] request_values = get_user_request_values();
        try{
            send_and_receive(request_values);
            System.out.println("request success");
        }
        catch(ApplicationException ae) {
            System.out.println("error: " + ae.getMessage());
        }
    }

    /** Generating for services
     * @param service_id the service requested
     * @param r connection info
     * @return the requested Service
     */
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
        else {
            return null;
        }
    }

    /** Wrapper for Util.send_and_receive
     * @param values parameter values for the service
     * @return the reply from the server, as a Map
     * @throws IOException send/receive message
     * @throws ApplicationException BadPathnameException, BadRangeException, FileEmptyException
     */
    public Map<String, Object> send_and_receive(String[] values) throws IOException, ApplicationException {
        return Util.send_and_receive(service_id, values, runner);
    }

    /** Gets values required to perform Service from user
     * @return the values
     */
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
