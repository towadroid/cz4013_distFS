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

        boolean cached = cache_object.has_range(offset, byte_count);
        boolean local_fresh = cache_object.validate_local();
        boolean server_fresh = true;
        if (!local_fresh) {
            server_fresh = cache_object.validate_server(runner);
        }

        if (!cached || cached && !local_fresh && !server_fresh) {
            Map<String, Object> reply = send_and_receive(request_values);
            cache_object.set_cache(offset, byte_count, (String) reply.get("content"));
        }

        String content = cache_object.get_cache(offset, byte_count);
        System.out.println(content);

    }

}
