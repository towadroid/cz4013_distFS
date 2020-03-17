public abstract class Service {

    public Runner runner;

    public Service(Runner r) {
        runner = r;
    }

    public abstract void act();

    public static Service generate_service(int service_id, Runner r) {
        if (service_id == Constants.READ_REQUEST_ID) {
            return new Read(r);
        }
        else {
            return null;
        }
    }

}
