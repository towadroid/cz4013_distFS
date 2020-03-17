public class Read extends  Service {

    public Read(Runner r) {
        super(r);
    }

    @Override
    public void act() {
        //TODO: ask for user input, send/receive message
        String request_param_string = Util.get_request_param_string(Constants.READ_REQUEST_ID);
        System.out.println(request_param_string);
        String request_value_string = runner.scanner.nextLine();



    }

}
