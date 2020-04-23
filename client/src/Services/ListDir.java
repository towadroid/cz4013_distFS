package Services;

import Exceptions.ApplicationException;
import Helpers.Constants;
import Helpers.Runner;

import java.io.IOException;
import java.util.Map;

public class ListDir extends Service {

    public ListDir(Runner r) {
        super(r);
        service_id = Constants.LIST_ID;
    }

    /** Perform the service
     * @throws IOException send/receive message
     */
    @Override
    public void act() throws IOException {
        //ask for user input
        String[] request_values = get_user_request_values();
        try{
            Map<String, Object> reply = send_and_receive(request_values);
            int repeat = (int) reply.get("repeat");
            for (int i = 0; i < repeat; i++) {
                String type_key = "type";
                String name_key = "name";
                if (repeat > 1) {
                    type_key += " " + i;
                    name_key += " " + i;
                }

                String type = "";
                if ((int) reply.get(type_key) == 1) {
                    type = "dir: ";
                }
                else{
                    type = "file:";
                }
                String name = (String) reply.get(name_key);
                System.out.println(type + " " + name);
            }
            System.out.println("Done.");
        }
        catch(ApplicationException ae) {
            System.out.println("Error: " + ae.getMessage() + ".");
        }
    }

}
