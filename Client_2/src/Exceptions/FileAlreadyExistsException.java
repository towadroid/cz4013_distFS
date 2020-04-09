package Exceptions;

public class FileAlreadyExistsException extends ApplicationException {
    public FileAlreadyExistsException ()  {
        super("file already exists");
    }
}
