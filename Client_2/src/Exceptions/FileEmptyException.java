package Exceptions;

public class FileEmptyException extends ApplicationException{
    public FileEmptyException() {
        super("file is empty");
    }
}
