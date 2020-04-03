package Exceptions;
import Helpers.Constants;

public class ApplicationException extends Exception {
    public ApplicationException (String m) {
        super(m);
    }
    public static void check_app_exception(int alert_id) throws ApplicationException {
        if (alert_id == Constants.NO_SUCH_FILE_ID) {
            throw new BadPathnameException();
        }
        if (alert_id == Constants.BAD_RANGE_ID) {
            throw new BadRangeException();
        }
        if (alert_id == Constants.FILE_EMPTY_ID) {
            throw new FileEmptyException();
        }
    }
}
