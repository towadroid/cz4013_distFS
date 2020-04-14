package Exceptions;

public class ServerBusyException extends ApplicationException {
    public ServerBusyException  ()  {
        super("server is busy; please try again later");
    }
}
