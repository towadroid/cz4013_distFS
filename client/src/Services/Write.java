package Services;

import Helpers.Constants;
import Helpers.Runner;

public class Write extends Service {

    public Write(Runner r) {
        super(r);
        service_id = Constants.WRITE_ID;
    }

}
