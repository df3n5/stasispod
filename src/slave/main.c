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

void put(char* id, char* data) {
    printf("put(\"%s\", \"%s\")\n", id, data);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);

    if(access(filename, F_OK) != -1 ) {
        printf("file exists, skipping write\n");
    } else {
        FILE* f = fopen(filename, "w");
        size_t bytes = fwrite(data, sizeof(char), strlen(data), f);
        fclose(f);
        printf("wrote %d bytes to %s\n", bytes, filename);
    }
}

void get(char* id) {
    printf("get(%s)\n", id);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);

    if(access(filename, R_OK) != -1 ) {
        char* file_buf = calloc(MAX_FILE_SIZE, sizeof(char));
        FILE* f = fopen(filename, "r");
        size_t bytes_read = fread(file_buf, sizeof(char), MAX_FILE_SIZE, f);
        file_buf[bytes_read] = '\0';

        printf("Printing file:\n");
        printf("%s\n", file_buf);
    } else {
        printf("cannot open %s\n", filename);
    }
}

void del(char* id) {
    printf("del(%s)\n", id);
    char filename[255];
    sprintf(filename, "%s/%s", DATA_DIR, id);

    if(access(filename, F_OK) != -1 ) {
        unlink(filename);
        printf("successfully removed %s\n", filename);
    } else {
        printf("cannot open %s\n", filename);
    }
}


void list() {
    printf("list()\n");
    DIR* dp;
    struct dirent* ep;
    dp = opendir(DATA_DIR);

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            puts(ep->d_name);
        }
        closedir(dp);
    } else {
        perror("could not open the data directory");
    }
}

void rebalance(void) {
    printf("rebalance()...\n");
}

void replicate() {
    printf("replicate()...\n");
}

int main(void) {
    zsock_t* rep = zsock_new_rep("tcp://127.0.0.1:5000");

    printf("Starting slave...\n");
    char choice[255];
    char id[255];
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

        json_decref(obj);
        zstr_free(&string);

        // TODO: Plug the values into this and get it to these based on them.

        // Send response
        // TODO
        zstr_send(rep, "Hello, World back at you");

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
    }

    zsock_destroy(&rep);
}
