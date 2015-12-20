#include <czmq.h>

static int MAX_SEND_SIZE = 134217728;

void make_request(char* func, char* id, char* data) {
    zsock_t* req = zsock_new_req("tcp://127.0.0.1:5000");
    char* send_buf = calloc(MAX_SEND_SIZE, sizeof(char));
    snprintf(send_buf, MAX_SEND_SIZE, "{\"func\": \"%s\", \"id\": \"%s\", \"data\": \"%s\"}", func, id, data);
    zstr_send(req, send_buf);
    //zstr_send(req, "{\"func\": \"g\", \"id\": \"idNet0\"}");
    //zstr_send(req, "{\"func\": \"ls\"}");
    //zstr_send(req, "{\"func\": \"d\", \"id\": \"idNet0\"}");
    //zstr_send(req, "{\"func\": \"d\", \"id\": \"idNet0\"}");

    char *string = zstr_recv(req);
    puts(string);
    zstr_free(&string);
    zsock_destroy(&req);
}

int main (void) {
    char choice[255];
    while(strcmp(choice, "q") != 0) {
        printf("press 'p' for put, 'g' for get, 'ls' for list dir, 'd' for delete, 'r' for rebalance and 'rep' for replicate: ");
        scanf("%s", choice);
        if(choice[0]=='p') {
            char id[255];
            char data[255];
            printf("id: ");
            scanf("%s", id);
            printf("data: ");
            scanf("%s", data);
            make_request("p", id, data);
        } else if(choice[0]=='g') {
            char id[255];
            printf("id: ");
            scanf("%s", id);
            make_request("g", id, "");
        } else if(choice[0]=='d') {
            char id[255];
            printf("id: ");
            scanf("%s", id);
            make_request("d", id, "");
        } else if(strcmp(choice, "ls") == 0) {
            make_request("ls", "", "");
        } else if(strcmp(choice, "r") == 0) {
            make_request("r", "", "");
        } else if(strcmp(choice, "rep") == 0) {
            make_request("rep", "", "");
        }
    }
}
