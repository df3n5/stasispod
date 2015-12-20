#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

#include <czmq.h>
#include <jansson.h>

static char* DATA_DIR = "data";
// 64 MB pow(2, 26)
static int MAX_FILE_SIZE = 67108864;
static int RESP_SIZE = 255;

char* put(const char* id, const char* data) {
    printf("put(\"%s\", \"%s\")\n", id, data);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);
    char* resp = calloc(RESP_SIZE, sizeof(char));

    if(access(filename, F_OK) != -1 ) {
        snprintf(resp, RESP_SIZE, "file exists, skipping write\n");
    } else {
        FILE* f = fopen(filename, "w");
        size_t bytes = fwrite(data, sizeof(char), strlen(data), f);
        fclose(f);
        snprintf(resp, RESP_SIZE, "wrote %d bytes to %s\n", bytes, filename);
    }
    return resp;
}

char* get(const char* id) {
    printf("get(%s)\n", id);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);
    char* resp = calloc(RESP_SIZE, sizeof(char));

    if(access(filename, R_OK) != -1 ) {
        char* file_buf = calloc(MAX_FILE_SIZE, sizeof(char));
        FILE* f = fopen(filename, "r");
        size_t bytes_read = fread(file_buf, sizeof(char), MAX_FILE_SIZE, f);
        file_buf[bytes_read] = '\0';

        printf("Printing file:\n");
        printf("%s\n", file_buf);
        snprintf(resp, RESP_SIZE, "%s", file_buf);
    } else {
        snprintf(resp, RESP_SIZE, "cannot open %s\n", filename);
    }
    return resp;
}

char* del(const char* id) {
    printf("del(%s)\n", id);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);
    char* resp = calloc(RESP_SIZE, sizeof(char));

    if(access(filename, F_OK) != -1 ) {
        unlink(filename);
        snprintf(resp, RESP_SIZE, "successfully removed %s\n", filename);
    } else {
        snprintf(resp, RESP_SIZE, "cannot open %s\n", filename);
    }
    return resp;
}

char* list() {
    printf("list()\n");
    DIR* dp;
    struct dirent* ep;
    dp = opendir(DATA_DIR);
    char* resp = calloc(RESP_SIZE, sizeof(char));

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if(ep->d_name[0] != '.') {
                strncat(resp, ep->d_name, RESP_SIZE);
                strncat(resp, "\n", RESP_SIZE);
            }
        }
        closedir(dp);
    } else {
        snprintf(resp, RESP_SIZE, "ERROR: could not open the data directory");
    }
    return resp;
}

char* rebalance(void) {
    char* resp = calloc(RESP_SIZE, sizeof(char));
    snprintf(resp, RESP_SIZE, "rebalance()...\n");
    return resp;
}

char* replicate() {
    char* resp = calloc(RESP_SIZE, sizeof(char));
    snprintf(resp, RESP_SIZE, "replicate()...\n");
    return resp;
}

int main(void) {
    zsock_t* rep = zsock_new_rep("tcp://127.0.0.1:5000");

    printf("Starting slave...\n");
    char choice[255];
    choice[0] = '\0';
    while(strcmp(choice, "q") != 0) {
        // Get request
        char* string = zstr_recv(rep);
        printf("recv : %s\n", string);

        json_error_t error;
        json_t* obj = json_loads(string, 0, &error);
        if(!obj) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return 1;
        }
        json_t* type_obj = json_object_get(obj, "func");
        if(!json_is_string(type_obj)) {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return 1;
        }

        const char* type = json_string_value(type_obj);
        printf("Type is : %s", type);
        fflush(stdout);
        strcpy(choice, type);

        if(choice[0]=='p') {
/*
            char id[255];
            char data[255];
            printf("id: ");
            scanf("%s", id);
            printf("data: ");
            scanf("%s", data);
*/
            json_t* id_obj = json_object_get(obj, "id");
            if(!json_is_string(id_obj)) {
                fprintf(stderr, "error: getting ID, line %d: %s\n", error.line, error.text);
                return 1;
            }
            const char* id = json_string_value(id_obj);

            json_t* data_obj = json_object_get(obj, "data");
            if(!json_is_string(data_obj)) {
                fprintf(stderr, "error: getting data, line %d: %s\n", error.line, error.text);
                return 1;
            }
            const char* data = json_string_value(data_obj);

            char* tmpresp = put(id, data);
            zstr_send(rep, tmpresp);
            free(tmpresp);
        } else if(choice[0]=='g') {
            //printf("id: ");
            //scanf("%s", id);

            json_t* id_obj = json_object_get(obj, "id");
            if(!json_is_string(id_obj)) {
                fprintf(stderr, "error: getting ID, line %d: %s\n", error.line, error.text);
                return 1;
            }
            const char* id = json_string_value(id_obj);

            char* tmpresp = get(id);
            zstr_send(rep, tmpresp);
            free(tmpresp);
        } else if(choice[0]=='d') {
            json_t* id_obj = json_object_get(obj, "id");
            if(!json_is_string(id_obj)) {
                fprintf(stderr, "error: getting ID, line %d: %s\n", error.line, error.text);
                return 1;
            }
            const char* id = json_string_value(id_obj);

            char* tmpresp = del(id);
            zstr_send(rep, tmpresp);
            free(tmpresp);
        } else if(strcmp(choice, "ls") == 0) {
            char* tmpresp = list();
            zstr_send(rep, tmpresp);
            free(tmpresp);
        } else if(strcmp(choice, "r") == 0) {
            char* tmpresp = rebalance();
            zstr_send(rep, tmpresp);
            free(tmpresp);
        } else if(strcmp(choice, "rep") == 0) {
            char* tmpresp = replicate();
            zstr_send(rep, tmpresp);
            free(tmpresp);
        }

        // TODO: Plug the values into this and get it to these based on them.

        // Send response
        // TODO
        //zstr_send(rep, "Hello, World back at you");

/*
        printf("press 'p' for put, 'g' for get, 'ls' for list dir, 'd' for delete, 'r' for rebalance and 'rep' for replicate: ");
        scanf("%s", choice);
        if(choice[0]=='p') {
            char id[255];
            char data[255];
            printf("id: ");
            scanf("%s", id);
            printf("data: ");
            scanf("%s", data);
            put(id, data);
        } else if(choice[0]=='g') {
            printf("id: ");
            scanf("%s", id);
            get(id);
        } else if(choice[0]=='d') {
            printf("id: ");
            scanf("%s", id);
            del(id);
        } else if(strcmp(choice, "ls") == 0) {
            list();
        } else if(strcmp(choice, "r") == 0) {
            rebalance();
        } else if(strcmp(choice, "rep") == 0) {
            replicate();
        }
*/
        json_decref(obj);
        zstr_free(&string);
        //char resp[255];
        //resp[0] = '\0';

    }

    zsock_destroy(&rep);
}
