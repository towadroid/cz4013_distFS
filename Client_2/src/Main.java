import Helpers.Constants;
import Helpers.Runner;
import Services.Service;

import java.io.IOException;

public class Main {

    // args: [ip address, port number]
    public static void main(String[] args) throws IOException {
        Runner runner = new Runner(args[0], Integer.parseInt(args[1]));
        while (true) {
            System.out.println(Constants.SERVICE_PROMPT);
            int input = Integer.parseInt(runner.scanner.nextLine());
            if (input == Constants.EXIT_ID) {
                break;
            }
            Service requested_service = Service.generate_service(input, runner);
            if (!(requested_service==null)) {
                requested_service.act();
            }
            System.out.println();
        }
        runner.socket.close();
    }

}
