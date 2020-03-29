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
        /* la funzione scanf() prende input da tastiera e lo mette nel buffer string
        %s specifica che il buffer e' una stringa*/
        scanf("%s", string);
        send(sock, string, strlen(string) , 0 ); 
        if(strcmp(string, "quit")==0){
            printf("QUIT SIIIIII\n");
            stop = 1;
            break;
        }
        printf("Message sent\n"); 
    }
}


int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in serv_addr; 
    /*La funzione socket() crea un socket e ne restituisce un descrittore. 
    Se il valore del descrittore e' pari a "-1", allora la creazione del 
    socket non e' andata a buon fine.
     
    AF_INET specifica che la famiglia di protocolli da usare è quella di Internet
    SOCK_STREAM specifica che la connessione deve essere permanente e bidirezionale 
    basata su un flusso di byte (i dati vengono mantenuti in ordine e non sono 
    persi o duplicati)
    
    0 dice di usare il protocollo di default per la combinazione dei 2 parametri 
    precedenti (in questo caso il protocollo di default e' TCP)*/
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // inet_pton() converte gli indirizzi IPv4 e IPv6 da testo in binario 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
   /* la funzione connect() cerca di effettuare la connessione fra il socket sock 
   passato come parametro e il socket in ascolto all'indirizzo specificato 
   
   sock è il descrittore della socket
   
   &serv_addr è l indirizzo del server
   
   sizeof(serv_addr) è la lunghezza dell'indirizzo del server
   */
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    pthread_t id;
    /*La funzione pthread_create() fa partire un nuovo thread dal processo chiamante.
    &id è il riferimento al thread creato 
    
    NULL significa che non vogliamo dare attributi al nuovo thread
    
    client_handler e' la funzione richiamata quando parte il thread
    
    NULL significa che non vogliamo passare argomenti alla funzione client_handler
    */
    if (pthread_create(&id, NULL, (void *)client_handler, NULL) != 0) {
        perror("Create pthread error!\n");
        exit(1);
    }

    while(1){
        int valread;
        char buffer[1024] = {0}; 
        
        /* Estrae 1024 caratteri dalla socket sock e li memorizza in buffer
        
        valread e' il numero di byte letti
        */
        valread = read(sock, buffer, 1024); 
        if(strlen(buffer)!=0){
            printf("%s\n",buffer);
        }
        if(stop == 1){
            break;
        }
    }
    return 0; 
} 
