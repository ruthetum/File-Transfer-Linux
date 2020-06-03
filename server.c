#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1460
#define MAX_DIGIT 15

void error_handling(char *msg);

int main(int argc, char *argv[]) {
  
  int server;               // Server socket
  int client;               // Client socket
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size;
  
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
  server = socket(PF_INET, SOCK_STREAM, 0); // IPv4, TCP
  if (server == -1)
    error_handling("socket() Error");
  
  // Init
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
  
  // Bind
  if (bind(server, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    error_handling("bind() Error");
  
  // Listen
  if (listen(server, 5) == -1)
    error_handling("listen() Error");
  printf("Server start!\n");
  
  // Accept
  clnt_addr_size = sizeof(clnt_addr);
  client = accept(server, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
  if (client == -1)
    error_handling("accept() Error");
  else
    printf("Connected Client!\n");
  
  // Client Message
  msg_size = read(client, clnt_msg, BUFFER_SIZE);
  printf("Client : %s\n", clnt_msg);
  
  // Open target
  checkFile = fopen("sample.mp4", "rb");
  if (checkFile == NULL) {
    fputs("File Error", stderr);
    exit(1);
  }
  
  // Send target size to client
  fseek(checkFile, 0, SEEK_END);
  file_size = ftell(checkFile);
  
  printf("Total File Size : %d\n", file_size);
  sprintf(str_file_size, "%d", file_size);  // integer -> string
  write(client, str_file_size, strlen(str_file_size));
  printf("Sending File Size to Client!\n");
  
  fclose(checkFile);
  
  readFile = fopen("sample.mp4", "rb");
  // Send target data to client
  while(!feof(readFile)) {
    recv_size = fread(buff, sizeof(char), BUFFER_SIZE, readFile);
    cur_size += recv_size;
    send(client, buff, recv_size, 0);
    
    ++index;
    if (index % 100 == 0)
      printf("Progressing : %d / %d\n", cur_size, file_size);
  }
  
  // Close
  fclose(readFile);
  close(client);
  close(server);
  printf("Successfully Send! : %d / %d\n", cur_size, file_size);
  
  return 0;
}

void error_handling(char *msg) {
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}