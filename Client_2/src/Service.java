import java.io.IOException;

public abstract class Service {

    public Runner runner;

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

}
