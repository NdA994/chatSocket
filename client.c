/*#include <unistd.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <pthread.h> 

int sockfd = 0;

int main(int argc, char const *argv[]) { 
    
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(8080); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    
    valread = read( sock , buffer, 1024); 


    return 0;
}*/

// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <arpa/inet.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#define PORT 8080 

int sock = 0;
int stop = 0;


void client_handler() {
    while(1){
        char string[256];
        scanf("%s", string);
        printf("%s\n", string);
        send(sock, string, strlen(string) , 0 ); 
        if(strcmp(string, "quit")==0){
            printf("QUIT SIIIIII\n");
            stop = 1;
        }
        printf("Message sent\n"); 
    }
}


int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in serv_addr; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    pthread_t id;
    if (pthread_create(&id, NULL, (void *)client_handler, NULL) != 0) {
        perror("Create pthread error!\n");
        exit(1);
    }

    while(1){
        int valread;
        char buffer[1024] = {0}; 
        valread = read(sock, buffer, 1024); 
        if(strlen(buffer)!=0){
            printf("%s\n",buffer );
        }
        if(stop == 1){
            printf("STOP");
            break;
        }
    }
    return 0; 
} 