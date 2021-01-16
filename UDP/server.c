#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 1460
#define MAX_DIGIT 15

void error_handling(char *msg); // function for error handling

int main(int argc, char *argv[]) {
  
  int server;               // Server socket
  int client;               // Client socket
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size;
  int addrlen = sizeof(struct sockaddr);
  
  char buff[BUFFER_SIZE];   // Buffer
  FILE * readFile;
  FILE * checkFile;
  int file_size = 0;
  char str_file_size[MAX_DIGIT];
  int recv_size = 0;
  int cur_size = 0;
  
  int msg_size;
  char clnt_msg[30];
  int index = 0;
  
  // Usage
  if (argc!=2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }
  
  // Make socket
  server = socket(PF_INET, SOCK_DGRAM, 0); // IPv4, UDP
  if (server == -1)
    error_handling("socket() Error");
  
  // Init (address,port)
  memset(&serv_addr, 0, sizeof(serv_addr));
  memset(&clnt_addr, 0, sizeof(clnt_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
  
  // Bind
  if (bind(server, (struct sockaddr*) &serv_addr, addrlen) == -1)
    error_handling("bind() Error");
  
  
  printf("Server start!\n");
 
  while (1) {
    msg_size = recvfrom(server, buff, BUFFER_SIZE , 0, (struct sockaddr *) &clnt_addr, &addrlen);
    if(msg_size< 0) {
        perror("recvfrom fail");
        exit(1);
    }
    buff[msg_size] = 0;

    if(!strncmp(buff, "helloserver", 10)) {
      break;
    }
  }
  
  // Measure the size of a file
  checkFile = fopen("sample.mp4", "rb");
  if (checkFile == NULL) {
    fputs("File Error", stderr);
    exit(1);
  }
  fseek(checkFile, 0, SEEK_END);
  file_size = ftell(checkFile);
  
  // Send the size to the client
  printf("Total File Size : %d\n", file_size);
  sprintf(str_file_size, "%d", file_size);  // integer -> string
  write(client, str_file_size, strlen(str_file_size));
  printf("Send File Size to Client!\n");
  fclose(checkFile);
  
  // Measure working time
  clock_t start = clock();
  // Send the file to the client
  readFile = fopen("sample.mp4", "rb");
  while(!feof(readFile)) {
    recv_size = fread(buff, sizeof(char), BUFFER_SIZE, readFile);
    cur_size += recv_size;
    sendto(server, buff, BUFFER_SIZE, 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));
    
    ++index;
    if (index % 100 == 0)
      printf("Progressing : %d / %d\n", cur_size, file_size);
  }
  clock_t end = clock();
  
  // Close
  fclose(readFile);
  close(client);
  close(server);
  
  printf("\n");
  printf("Successfully Send! : %d / %d\n", cur_size, file_size);
  printf("Working Time : %lf sec\n", (double)(end - start)/CLOCKS_PER_SEC);
  
  return 0;
}

void error_handling(char *msg) {
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}