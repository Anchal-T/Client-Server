#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

void send_file(FILE *fp, int socketf){
	int n;
	char data[1024] = {0};

	while(fgets(data, 1024, fp) != NULL){
		if(send(socketf, data, sizeof(data), 0) == -1){
			return;
		}
		bzero(data, 1024);
	}
}

int main(){
	FILE *fp;
	char *filename = "file.txt";

	int socketf =  socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in address = 
	{
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

	connect(socketf, (const struct sockaddr *)&address, sizeof(address));

	struct pollfd fds[2] = {
		{
			0,
			POLLIN,
			0
		},
		{
			socketf,
			POLLIN,
			0
		}
	};
	fp = fopen(filename, "r");
	// send_file(fp, socketf);

	for(;;){
		char buffer[256] = { 0 };

		poll(fds, 2, 50000);

		if(fds[0].revents & POLLIN){
			read(0, buffer, 255);
			send(socketf, buffer, 255, 0);
		}
		else if(fds[1].revents & POLLIN){
			if(recv(socketf, buffer, 255, 0) == 0){
				return 0;
			}
			printf("%s\n", buffer);
		}
	}
}
