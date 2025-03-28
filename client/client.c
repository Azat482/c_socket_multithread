#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>


#define COMMAND_LENGTH 1000
#define SERVER_RESPONSE_LENGTH 1000


void buffer_cleaner(char *buff, int len){
    int i;
    for(i=0; i < len; i++){
        buff[i] = '\0';
    }
}


int main(int argc, char *argv[]){

    int socket_d;
    int connect_serv;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4321);                     
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    socket_d = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_d < 0){
        perror("client socket");
        exit(-1);
    }

    connect_serv = connect(socket_d, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(connect_serv < 0){
        perror("server connection");
        exit(-2);
    }
    printf("Connected to server:\n");
    
    int action_counter;
    for(action_counter = 0; ;action_counter++){
        char command_buffer[COMMAND_LENGTH];
        char server_res_buffer[SERVER_RESPONSE_LENGTH];
        buffer_cleaner(command_buffer, sizeof(command_buffer));
        buffer_cleaner(server_res_buffer, sizeof(server_res_buffer));
        printf("$server[%d]--> ", action_counter);
        fgets(command_buffer, COMMAND_LENGTH, stdin);
        command_buffer[strcspn(command_buffer, "\n")] = '\0'; //delete \n 
        if(!strcmp(command_buffer, "--exit")){   
            close(socket_d);
            break;
        }
        send(socket_d, command_buffer, strlen(command_buffer), 0);
        recv(socket_d, server_res_buffer, sizeof(server_res_buffer), 0);
        printf("Server response:\n%s\n", server_res_buffer);
    }
    
    return 0;
}