#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <pthread.h>
#include "database_mng.h"


#define NUM_THREADS 10
#define BACKLOG      5
#define RECIEVER_BUFFER_SIZE 100
#define SERVER_RESPONSE_BUFFER_SIZE 1000
#define TEXT_VALUE_BUFFER_SIZE 1000
#define VARIABLE_NAME_BUFFER_SIZE 50


void buffer_cleaner(char *buff, int len){
    int i;
    for(i=0; i < len; i++){
        buff[i] = '\0';
    }
}


void request_handler(void* client_sd){
    int client_socket_d = *(int *)client_sd;
    char recv_bytes[RECIEVER_BUFFER_SIZE];
    char resp_bytes[SERVER_RESPONSE_BUFFER_SIZE];
    char variable_name[VARIABLE_NAME_BUFFER_SIZE];
    char text_value[TEXT_VALUE_BUFFER_SIZE];
    buffer_cleaner(recv_bytes, sizeof(recv_bytes));
    buffer_cleaner(resp_bytes, sizeof(resp_bytes));
    buffer_cleaner(variable_name, sizeof(variable_name));
    buffer_cleaner(text_value, sizeof(text_value));
    int recv_bytes_n;
    buffer_cleaner(resp_bytes, sizeof(resp_bytes));
    buffer_cleaner(text_value, sizeof(text_value));
    printf("Client [%d] succesfully connected\n", client_socket_d);
    while(1){
        buffer_cleaner(recv_bytes, sizeof(recv_bytes));
        recv_bytes_n = recv(client_socket_d, recv_bytes, sizeof(recv_bytes), 0);
        if(recv_bytes_n <= 0){
            printf("Client[%d], disconnected\n", client_socket_d, recv_bytes_n);
            break;
        }
        printf("command [recieved bytes number = %d]:\t%s\n", recv_bytes_n, recv_bytes);
        if(!strcmp(recv_bytes, "--create")){
            buffer_cleaner(variable_name, sizeof(variable_name));
            buffer_cleaner(text_value, sizeof(text_value));
            strcpy(resp_bytes, "Enter name of the variable:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, variable_name, sizeof(variable_name), 0);
            strcpy(resp_bytes, "Enter the text:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, text_value, sizeof(text_value), 0);
            //database operations...
            printf("variable name: %s\ttext value: %s\n", variable_name, text_value);
            int crv = create_var(variable_name, text_value);
            //database operations end/
            if(crv > 0){
                strcpy(resp_bytes, "Operation [CREATE] finish.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            else{
                strcpy(resp_bytes, "Operation [CREATE] ERROR.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
        }
        else if(!strcmp(recv_bytes, "--update")){
            buffer_cleaner(variable_name, sizeof(variable_name));
            buffer_cleaner(text_value, sizeof(text_value));
            strcpy(resp_bytes, "Enter name of the variable:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, variable_name, sizeof(variable_name), 0);
            strcpy(resp_bytes, "Enter the new text:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, text_value, sizeof(text_value), 0);
            //database operations...
            printf("variable name: %s\ttext value: %s\n", variable_name, text_value);
            int upv = update_var(variable_name, text_value);
            //database operations end/
            if(upv > 0){
                strcpy(resp_bytes, "Operation [UPDATE] finish.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            else{
                strcpy(resp_bytes, "Operation [UPDATE] ERROR.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
        }
        else if(!strcmp(recv_bytes, "--delete")){
            buffer_cleaner(variable_name, sizeof(variable_name));
            buffer_cleaner(text_value, sizeof(text_value));
            strcpy(resp_bytes, "Enter name of the variable:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, variable_name, sizeof(variable_name), 0);
            //database operations...
            printf("variable name: %s\n", variable_name);
            int delv = delete_var(variable_name);
            //database operations end/
            if(delv > 0){
                strcpy(resp_bytes, "Operation [DELETE] finish.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            else{
                strcpy(resp_bytes, "Operation [DELETE] ERROR.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
        }
        else if(!strcmp(recv_bytes, "--get")){
            buffer_cleaner(variable_name, sizeof(variable_name));
            buffer_cleaner(text_value, sizeof(text_value));
            strcpy(resp_bytes, "Enter name of the variable:");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
            recv(client_socket_d, variable_name, sizeof(variable_name), 0);
            
            //database operations...
            printf("variable name: %s\n", variable_name);
            char getting_text_value[TEXT_VALUE_SIZE];
            int getv = get_var(variable_name, getting_text_value);
            //database operations end/
            if(getv > 0){
                strcpy(resp_bytes, "VALUE: ");
                strcat(resp_bytes, getting_text_value);
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            else{
                strcpy(resp_bytes, "Operation [GET] not succesfuly.");
                send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            }
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
        }
        else{
            strcpy(resp_bytes, "Invalid command: ");
            send(client_socket_d, resp_bytes, strlen(resp_bytes), 0);
            buffer_cleaner(resp_bytes, sizeof(resp_bytes));
        }
    }
    close(client_socket_d);
}


int main(int argc, char * argv[]){
    int socket_d;
    int socket_binded;
    int socket_listener;
    int clients_arr_socket_d[NUM_THREADS];
    pthread_t threads_arr[NUM_THREADS];

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4321);                     
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    socket_d = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_d < 0){
        perror("server socket");
        exit(-1);
    }

    socket_binded = bind(socket_d, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(socket_binded < 0){
        perror("socket bind");
        exit(-2);
    }

    socket_listener = listen(socket_d, BACKLOG);
    if(socket_listener < 0){
        perror("socket listener");
        exit(-3);
    }else{
        printf("Waiting...\n");
    }

    while (1){
        int thread_index;
        for (thread_index = 0; thread_index < NUM_THREADS; thread_index++){
            clients_arr_socket_d[thread_index] = accept(socket_d, NULL, NULL);
            pthread_create(&threads_arr[thread_index], NULL, (void *)request_handler, (void *)&clients_arr_socket_d[thread_index]);
            if (clients_arr_socket_d[thread_index] < 0){
                perror("client accept");
                exit(-4);
            }
        }
    }

    return 0;
}