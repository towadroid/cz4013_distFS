package Services;

import Helpers.Constants;
import Helpers.Runner;

public class Create extends Service{

    public Create(Runner r) {
        super(r);
        service_id = Constants.CREATE_FILE_ID;
    }

}
