package Exceptions;

public class CorruptMessageException extends ApplicationException {

    public CorruptMessageException() {
        super("Corrupt message received");
    }
}
