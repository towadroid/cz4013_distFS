package Services;

import Helpers.Constants;
import Helpers.Runner;

public class Remove extends Service {

    public Remove(Runner r) {
        super(r);
        service_id = Constants.REMOVE_FILE_ID;
    }

}
