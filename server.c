#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

// void siggchild_handeler(int s){
// 	int saved_errno = errno;
// 	while(waitpid(-1, NULL, WNOHANG) > 0);
// 	errno = saved_errno;
// }

void write_file(int socketf){
	int n;
	FILE *fp;
	char *filename = "recv.txt";
	char buffer[1024];

	fp = fopen(filename, "w");
	while(1){
		n = recv(socketf, buffer, 1024, 0);
		if(n <= 0){
			break;
			return;
		}
		fprintf(fp, "%s\n", buffer);
		bzero(buffer, 1024);
	}
	return;
}

int main(){
	int socketf =  socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address = 
	{
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY // Bind to all interfaces (0.0.0.0)	
    };

	bind(socketf, (const struct sockaddr *)&address, sizeof(address));
	listen(socketf, 10);
	int clinetf = accept(socketf, 0, 0);

	struct pollfd fds[2] = {
		{
			0,
			POLLIN,
			0
		},
		{
			clinetf,
			POLLIN,
			0
		}
	};
	// write_file(clinetf);
	send(clinetf, "Hello to this WORLD!", 20, 0);
	for(;;){
		char buffer[256] = { 0 };

		poll(fds, 2, 50000);

		if(fds[0].revents & POLLIN){
			read(0, buffer, 255);
			send(clinetf, buffer, 255, 0);
		}
		else if(fds[1].revents & POLLIN){
			if(recv(clinetf, buffer, 255, 0) == 0){
				return 0;
			}
			printf("%s\n", buffer);
		}
	}
}