#include <czmq.h>

int main (void)
{
    zsock_t* req = zsock_new_req("tcp://127.0.0.1:5000");
    zstr_send(req, "{\"func\": \"g\"}");

    char *string = zstr_recv(req);
    puts(string);
    zstr_free(&string);

    zsock_destroy(&req);
    return 0;
}
