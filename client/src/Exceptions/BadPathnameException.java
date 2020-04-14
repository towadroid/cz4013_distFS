package Exceptions;

public class BadPathnameException extends ApplicationException{

    public BadPathnameException() {
        super("pathname does not exist");
    }
}
