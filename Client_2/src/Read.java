import java.io.IOException;
import java.util.Map;

public class Read extends  Service {

    public Read(Runner r) {
        super(r);
        service_id = Constants.READ_ID;
    }

    @Override
    public void act() throws IOException {

        //ask for user input
        String[] request_values = get_user_request_values();
        String pathname = request_values[0];
        int offset = Integer.parseInt(request_values[1]);
        int byte_count = Integer.parseInt(request_values[2]);

        if (!runner.cache.containsKey(pathname)) {
            runner.cache.put(pathname, new CacheObject(pathname, runner));
        }

        CacheObject cache_object = runner.cache.get(pathname);
        if (cache_object.must_read_server(offset, byte_count, runner)) {
            Map<String, Object> reply = send_and_receive(request_values);
            cache_object.set_cache(offset, byte_count, (String) reply.get("content"));
        }

        String content = cache_object.get_cache(offset, byte_count);
        System.out.println(content);

    }

}
