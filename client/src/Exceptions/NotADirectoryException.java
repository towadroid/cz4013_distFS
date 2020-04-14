package Exceptions;

public class NotADirectoryException extends ApplicationException {
    public NotADirectoryException()  {
        super("Not a directory");
    }
}
