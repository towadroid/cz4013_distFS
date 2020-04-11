package Services;

import Helpers.Constants;
import Helpers.Runner;

public class ListDir extends Service {

    public ListDir(Runner r) {
        super(r);
        service_id = Constants.LIST_ID;
    }

}
