#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define MAX 4096 
#define PORT 8080 
#define SA struct sockaddr 


// funciont that reads the users messages in chunks of 4096 chars
void *read_message(int *size){
    char **messages = calloc(1, sizeof(char *));
    messages[0] = calloc(MAX, sizeof(char));

    int n = 0;
    int i = 0;
    while ((messages[i][n++] = getchar()) != '\n'){
        printf("%d ", n);
        if(n == MAX){
            i++;
            messages = realloc(messages, sizeof(char *)*(i+1));
            messages[i] = calloc(MAX, sizeof(char));
            n = 0;
        }
    }
    messages[i][n-1] = '\0';
    
    *size = i+1;
    return messages;
}

// free the message buff
void free_messages(char **messages, int size){
    for(int i = 0; i < size; i++){
        free(messages[i]);
    }
    free(messages);
}


void chat(int socket_instance){ 
    char **messages; 
    int n; 
    int close = 0;
    int n_messages;
    char message[MAX];
    int size = 0;
    
    while (1) { 
        printf("\nEnter the message: "); 

        // gets the user message
        messages = read_message(&size);
        
        // check if the command to exit was entered 
        if(size == 1 && !strncmp("/quit", messages[0], 4))
            break;

        // send how messages will be sent to server side
        write(socket_instance, &size, sizeof(size)); 
        
        // send the messages
        for(int i = 0; i < size; i++)
            write(socket_instance, messages[i], MAX); 
        
        // free the buffer
        free_messages(messages, size);
        n_messages = 0;
        
        // read how many messages must recieve 
        while(n_messages == 0){
            read(socket_instance, &n_messages, sizeof(n_messages)); 
        }

        // read and print the messages
        for(int i = 0; i < n_messages; i++){
            bzero(message, MAX);
            read(socket_instance, message, MAX); 
            printf("\nFrom Server : %s", message); 
        }
    } 
} 
  

int main() { 
    int socket_instance, connfd; 
    struct sockaddr_in serveraddress; 
  
    // socket create and varification 
    socket_instance = socket(AF_INET, SOCK_STREAM, 0); 
    if (socket_instance == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&serveraddress, sizeof(serveraddress)); 

    char ip_address[18];
    printf("Please, enter the IP Address to connect\n");
    scanf("%[^\n]%*c", ip_address);
    printf("Connecting to %s... ", ip_address);

    // assign IP, PORT 
    serveraddress.sin_family = AF_INET; 
    serveraddress.sin_addr.s_addr = inet_addr(ip_address); 
    serveraddress.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(socket_instance, (SA*)&serveraddress, sizeof(serveraddress)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server...\n"); 
        

    chat(socket_instance); 

    close(socket_instance); 
} 