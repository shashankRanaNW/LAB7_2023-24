#include "header.h"

int PILANI = 1;
int GOA = 2;
int HYDERABAD = 3;
#define NUM_CLIENTS 1024

int pilaniSocketId, goaSocketId, hydSocketId;
int myCampus;

// set up server to listen
int serv_set_up(char *addr, int port)
{
    int server_sockfd;
    int yes = 1;
    struct sockaddr_in server_addrinfo;

    // 1. SOCKET
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // SockOptions
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        close(server_sockfd);
        exit(1);
    }

    server_addrinfo.sin_family = AF_INET;
    server_addrinfo.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &server_addrinfo.sin_addr) <= 0)
    {
        perror("addr");
        close(server_sockfd);
        exit(1);
    }

    // 2. BIND
    if (bind(server_sockfd, (struct sockaddr *)&server_addrinfo, sizeof(server_addrinfo)) == -1)
    {
        perror("bind");
        close(server_sockfd);
        exit(1);
    }

    // 3. LISTEN
    if (listen(server_sockfd, NUM_CLIENTS) == -1)
    {
        perror("listen");
        close(server_sockfd);
        exit(1);
    }

    return server_sockfd;
}

// Accept incoming connections
int accept_clients(int socket_id)
{
    int new_server_sockfd;
    socklen_t sin_size;
    struct sockaddr_in client_addrinfo;

    sin_size = sizeof(client_addrinfo);

    // 4. ACCEPT
    new_server_sockfd = accept(socket_id, (struct sockaddr *)&client_addrinfo, &sin_size);
    if (new_server_sockfd == -1)
    {
        perror("accept");
        close(socket_id);
        exit(1);
    }

    return new_server_sockfd;
}

// Create a connection to the server
int conn_to_server(char *addr, int port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    int connect_status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_status < 0)
    {
        fprintf( stdout, "Could not find server\n");
        fflush( stdout );
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

// Pilani server will first start executing and listen on its Port 1 for Goa Server and Port 2 for Hyd Server
int serverSetupPilani(char* addr, int goaPort, int hydPort){
    myCampus = PILANI;
    //code for server setup
    int goaServSocketId = serv_set_up(addr, goaPort);
    int goaSocketId = accept_clients(goaServSocketId);
    int hydServSocketId = serv_set_up(addr, hydPort);
    int hydSocketId = accept_clients(hydServSocketId);

    return 1;
}
int serverSetupGoa(char* addr, int pilaniPort, int hydPort){
    myCampus = GOA;
    //code for server setup
    pilaniSocketId = conn_to_server(addr, pilaniPort);
    int hydServSocketId = serv_set_up(addr, hydPort);
    int hydSocketId = accept_clients(hydServSocketId);
    return 1;
    
}
int serverSetupHyderabad(char* addr, int pilaniPort, int goaPort){
    myCampus = HYDERABAD;
    //code for server setup
    pilaniSocketId = conn_to_server(addr, pilaniPort);
    goaSocketId = conn_to_server(addr, goaPort);
    return 1;   
}

int main(int argc, char *argv[])
{
    if (argc != 5)
	{
		printf("Refer Qn for arguments\n");
		return -1;
	}
    char* addr = argv[1];
	int port1 = atoi(argv[2]);
    int port2 = atoi(argv[3]);
    int port3 = atoi(argv[4]);
    int campus = atoi(argv[5]);

    if(campus == PILANI){
        serverSetupPilani(addr, port1, port2);
    }
    else if(campus == GOA){
        serverSetupGoa(addr, port1, port2);
    }
    else if(campus == HYDERABAD){
        serverSetupHyderabad(addr, port1, port2);
    }
    else{
        printf("Invalid campus\n");
        return -1;
    }

    //server setup should be done by now
    //start listening for department connections and subsequently create threads to handle them
    

}



