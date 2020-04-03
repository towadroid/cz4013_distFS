package Services;

import Helpers.Constants;
import Helpers.Runner;

public class Clear extends Service {

    public Clear(Runner r) {
        super(r);
        service_id = Constants.CLEAR_ID;
    }

}
