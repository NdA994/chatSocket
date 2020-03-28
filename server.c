
// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#define PORT 8080 

typedef struct ClientNode {
    int data;
    struct ClientNode* prev;
    struct ClientNode* link;
    //char ip[16];
} ClientList;

ClientList *newNode(int sockfd) {
    ClientList *np = (ClientList *)malloc( sizeof(ClientList) );
    np->data = sockfd;
    np->prev = NULL;
    np->link = NULL;
    //strncpy(np->ip, ip, 16);
    return np;
}

int count;
int server_fd, valread; 
char buffer[1024] = {0}; 
char *hello = "Hello from server"; 
ClientList *root, *now;

void send_to_all_clients(ClientList *np, char tmp_buffer[]) {
    ClientList *tmp = root->link;
    while (tmp != NULL) {
        if (np->data != tmp->data) { // all clients except itself.
            printf("Send to sockfd %d: \"%s\" \n", tmp->data, tmp_buffer);
            send(tmp->data, tmp_buffer, 1024, 0);
        }
        tmp = tmp->link;
    }
}


void client_handler(void *p_client) {
    ClientList *np = (ClientList *)p_client;
    while(1){
        valread = read(np->data, buffer, 1024); 
        if (valread > 0){
            printf("%s\n",buffer); 
            send_to_all_clients(np, buffer);
            //send(np->data, hello, strlen(hello), 0); 
            //printf("Hello message sent\n"); 
        }
    }
    
}

int main(int argc, char const *argv[]) { 
    
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    if (listen(server_fd, 10) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    root = newNode(server_fd);
    now = root;

    while(1){
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

        ClientList *c = newNode(new_socket);
        c->data = new_socket;
        c->prev = now;
        now->link = c;
        now = c;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(EXIT_FAILURE);
        }

        
    }
    return 0; 
} 
