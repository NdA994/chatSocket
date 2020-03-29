#include <unistd.h> 
#include <stdio.h> 
#include <pthread.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 

// dichiaro la struttura di un nodo
typedef struct ClientNode {
    //data conterra' il descrittore di file della socket
    int data;
    // prev e' il puntatore al nodo precedente
    struct ClientNode* prev;
    // link e' il puntatore al nodo successivo
    struct ClientNode* link;
} ClientList;

//creo un nuovo nodo da aggiungere successivamente alla lista
ClientList *newNode(int sockfd) {
    // alloco in memoria spazio per il nuovo nodo
    ClientList *np = (ClientList *)malloc( sizeof(ClientList) );
    np->data = sockfd;
    /*np->prev e np->link sono messi a NULL perche' qui sto solo creando il nodo
    ma lo leghero' alla lista (specificando il nodo precedente e quello successivo nella lista) 
    rispetto al nodo appena creato solo successivamente
    */
    np->prev = NULL;
    np->link = NULL;
    //ritorno un riferimento al nodo appena creato
    return np;
}

int count;
int server_fd;  
char *hello = "Hello from server"; 
ClientList *root, *now;

void send_to_all_clients(ClientList *np, char tmp_buffer[]) {
    /*inizializzo il puntatore temporaneo tmp a puntare al nodo 
    nella lista successivo al nodo radice*/
    ClientList *tmp = root->link;
    
    //Ciclo finche' non scorro tutta la lista 
    while (tmp != NULL) {
    /*se il nodo sotto analisi NON e' quello che ha come socket quella che 
    sta mandando i dati allora mando i dati su quella socket sotto analisi
    */
        if (np->data != tmp->data) { 
            send(tmp->data, tmp_buffer, 1024, 0);
        }
        //punto con il puntatore temporaneao al prossimo nodo nella lista
        tmp = tmp->link;
    }
}

void client_handler(void *p_client) {
    // typecasting del puntatore a void a ClientList
    ClientList *np = (ClientList *)p_client;
    /* la funzione send() manda un messaggio hello di lunghezza strlen(hello) 
    sulla socket np->data
    
    0 significa che nn vogliamo usare i flags
    */
    send(np->data , hello , strlen(hello) , 0 ); 
    while(1){
        char buffer[1024] = {0}; 
        printf("Hello message sent\n"); 
        /* Estrae 1024 caratteri dalla socket np->data  e li memorizza in buffer
        
        valread e' il numero di byte letti
        */
        int valread = read(np->data , buffer, 1024); 
        printf("%s\n",buffer );   
        send_to_all_clients(np, buffer);
        if(strcmp(buffer, "quit")==0){
            break;
        }
    } 
    //chiude la socker np->data
    close(np->data);
    if (np == now) { // rimuovi nodo da un bordo della lista
        now = np->prev;
        now->link = NULL;
    } else { // rimuovi nodo dal mezzo della lista
        np->prev->link = np->link;
        np->link->prev = np->prev;
    }
    /* la funzione free() rilascia la memoria precedentemente allocata 
    per quel nodo np in maniera dinamica con la funzione malloc()*/
    free(np);
}

int main(int argc, char const *argv[]) { 
    int server_fd, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    /* imposta il primo carattere nel buffer a NULL. Tale tecnica viene comunemente utilizzata 
    per le stringhe con terminazione NULL, poiché tutti i dati oltre il primo NULL vengono ignorati 
    dalle successive funzioni che gestiscono stringhe con terminazione NULL.
    */
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    /*La funzione socket() crea un socket e ne restituisce un descrittore. 
    Se il valore del descrittore e' pari a "-1", allora la creazione del 
    socket non e' andata a buon fine.
     
    AF_INET specifica che la famiglia di protocolli da usare è quella di Internet
    
    SOCK_STREAM specifica che la connessione deve essere permanente e bidirezionale 
    basata su un flusso di byte (i dati vengono mantenuti in ordine e non sono 
    persi o duplicati)
    
    0 dice di usare il protocollo di default per la combinazione dei 2 parametri 
    precedenti (in questo caso il protocollo di default e' TCP)*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
       
    /* La funzione setsockopt() manipola le opzioni della socket 
    riferita dal descrittore di file server_fd
    
    SOL_SOCKET è usato per manipolare le opzioni a livello di API socket
    
    SO_REUSEADDR e SO_REUSEPORT sono simili e queste opzioni ci permettono di 
    fare il bind di diverse server sockets TCP/UDP sullo stesso indirizzi IP e porto.
    */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    
    //INADDR_ANY indica al socket di ascolto di collegarsi a tutte le interfacce disponibili
    address.sin_addr.s_addr = INADDR_ANY; 
    /*la funzione htons () converte i valori tra gli ordini di byte host e di rete. 
    Esiste una differenza tra l'ordine dei byte big-endian e little-endian e di rete 
    in base alla macchina e al protocollo di rete in uso.*/
    address.sin_port = htons( PORT ); 
       
    // Collegamento forzato della socket alla porta 8080
    /* la funzione bind() assegna l'indirizzo &address al socket riferito 
    dal descrittore di file server_fd.
    
    sizeof(address) è la lunghezza dell'indirizzo
    */
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    /* la funzioen listen() mette in ascolto la soket server_fd
    3 e' la lunghezza massima della coda con le connessioni pendenti 
    per la socket server_fd
    */
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    

    root = newNode(server_fd);
    now = root;

    
    
    while(1){
        int new_socket;
        /* Estrae la prima richiesta di connessione sulla coda delle connessioni 
        in sospeso per il socket server_fd in ascolto, crea un nuovo socket connesso 
        e restituisce un nuovo descrittore di file new_socket che si riferisce a quel socket.
        
        L'argomento address è un puntatore a una struttura sockaddr. 
        Questa struttura è riempita con l'indirizzo del socket peer
        */
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

	/*c e' un puntatore ad un ClientList
	Semplicemente quete operazioni servono per aggiungere correttamente un nuovo Node nella lista
	*/
        ClientList *c = newNode(new_socket);
        c->data = new_socket;
        c->prev = now;
        now->link = c;
        now = c;

        pthread_t id;
        /*La funzione pthread_create() fa partire un nuovo thread dal processo chiamante.
    	
    	&id è il riferimento al thread creato 
    
    	NULL significa che non vogliamo dare attributi al nuovo thread
    
    	client_handler e' la funzione richiamata quando parte il thread
    
    	c e' l'argomento che passiamo alla funzione client_handler
    	*/
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(EXIT_FAILURE);
        }

    }

    return 0; 
} 
