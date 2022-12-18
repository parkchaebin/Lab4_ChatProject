#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAX_LINE  511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit"; 
char *START_STRING = "Connected to chat_server \n";

int max_fdp1;	
char ip_list[MAX_SOCK][20];
int listen_sock;			
void addClient(int s, struct sockaddr_in *new_client_addr);    
int get_max();			
int tcp_listen(int host, int port, int backlog);
void err_quit(char *msg) { perror(msg); exit(1); }

time_t ct;
struct tm tm;



int main(int argc, char *argv[]) {
	struct sockaddr_in client_addr;
	char buf[MAX_LINE + 1]; 
	int i, j, nbyte, accp_sock, addrlen = sizeof(struct
		sockaddr_in);
	fd_set read_fds;	
	pthread_t a_thread;
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);
	pthread_create(&a_thread, NULL, thread_function, (void *)NULL);
	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		for (i = 0; i < num_user; i++)
			FD_SET(client_sock_list[i], &read_fds);

		max_fdp1 = get_max() + 1;	
		if (select(max_fdp1, &read_fds, NULL, NULL, NULL) < 0)
			err_quit("select fail");


		for (i = 0; i < num_user; i++) {
			if (FD_ISSET(client_sock_list[i], &read_fds)) {
				nbyte = recv(client_sock_list[i], buf, MAX_LINE, 0);
				if (nbyte <= 0) {
					removeClient(i);
					continue;
				}
				buf[nbyte] = 0;
		
				if (strstr(buf, EXIT_STRING) != NULL) {
					removeClient(i);	
					continue;
				}
			
				for (j = 0; j < num_user; j++)
					send(client_sock_list[j], buf, nbyte, 0);
				printf("%s", buf);			/
			}
		}
	}
	return 0;
}


void addClient(int s, struct sockaddr_in *new_client_addr) {
	char buf[20];
	inet_ntop(AF_INET, &new_client_addr->sin_addr, buf, sizeof(buf));
	printf("new client: %s\n", buf); 
	
	client_sock_list[num_user] = s;
	strcpy(ip_list[num_user], buf);
	num_user++; 
}



int get_max() {
	int max = listen_sock;
	int i;
	for (i = 0; i < num_user; i++)
		if (client_sock_list[i] > max)
			max = client_sock_list[i];
	return max;
}


int  tcp_listen(int host, int port, int backlog) {
	int sd;
	struct sockaddr_in server_addr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket fail");
		exit(1);
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(host);
	server_addr.sin_port = htons(port);
	if (bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind fail ");  exit(1);
	}

	listen(sd, backlog);
	return sd;
}



