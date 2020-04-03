package Services;

import Helpers.Constants;
import Helpers.Runner;

public class TestService extends Service {

    public TestService(Runner r) {
        super(r);
        service_id = Constants.WRITE_ID;
    }

}
