import Helpers.Constants;
import Helpers.Runner;
import Services.Service;

import java.io.IOException;
import java.util.Scanner;

public class Main {

    // args: [ip address, port number, at most once, network failure rate, freshness interval]
    public static void main(String[] args) throws IOException {
        Scanner scanner = new Scanner(System.in);
        Runner runner;
        String[] initial_input = args;
        while(true) {
            try {
                runner = new Runner(scanner, initial_input[0], Integer.parseInt(initial_input[1]),
                        Integer.parseInt(initial_input[2]), Double.parseDouble(initial_input[3]),
                        Integer.parseInt(initial_input[4]));
                break;
            }
            catch(ArrayIndexOutOfBoundsException e) {
                System.out.println("Bad parameters command line args. Please type: ");
                System.out.println("{ip address} {port number} " +
                        "{1 for at most once, 0 for at least once} " +
                        "{network failure rate} {freshness interval in milliseconds}");
                initial_input = scanner.nextLine().split(" ");
                System.out.println("");
            }
        }

        System.out.println("Welcome!");
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
        runner.close();
        System.out.println("Goodbye!");
    }

}
