#include <czmq.h>

int main (void)
{
    zsock_t* req = zsock_new_req("tcp://127.0.0.1:5000");
    //zstr_send(req, "{\"func\": \"p\", \"id\": \"idNet0\", \"data\": \"this is a network sent data string\"}");
    //zstr_send(req, "{\"func\": \"g\", \"id\": \"idNet0\"}");
    //zstr_send(req, "{\"func\": \"ls\"}");
    //zstr_send(req, "{\"func\": \"d\", \"id\": \"idNet0\"}");
    zstr_send(req, "{\"func\": \"d\", \"id\": \"idNet0\"}");

    char *string = zstr_recv(req);
    puts(string);
    zstr_free(&string);

    zsock_destroy(&req);
    return 0;
}
