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
  
  int sock;
  struct sockaddr_in serv_addr;
  
  char hsv[] = "helloserver";
  
  char buff[BUFFER_SIZE];
  FILE * writeFile;
  FILE * checkFile;
  int file_size = 0;
  char str_file_size[MAX_DIGIT];
  int packet_size = 0;
  int recv_size = 0;
  int cur_size = 0;
  int addrlen = sizeof(struct sockaddr);
  
  int index = 0;
  
  // Usage
  if (argc!=3) {
    printf("Usage : %s <IP> <PORT>\n", argv[0]);
    exit(1);
  }
  
  // Make socket
  sock = socket(PF_INET, SOCK_DGRAM, 0); // IPv4, TCP
  if (sock == -1)
    error_handling("socket() Error");
  
  // Init (address,port)
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));
  
  // Send message 'Hello Server' to the server
  sendto(sock, hsv, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  
  // Measure working time
  clock_t start = clock();
  // Read and Write
  writeFile = fopen("rece_udp.mp4", "wb");
  while ((recv_size = recvfrom(sock, buff, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addrlen)) != 0) {
    fwrite(buff, sizeof(char), recv_size, writeFile);
    cur_size += recv_size;
    
    ++index;
    if (index % 100 == 0)
      printf("Progressing : %d\n", cur_size);
  }
  clock_t end = clock();
  
  // Close
  fclose(writeFile);
  close(sock);
  
  printf("\n");
  // Compare size
  if (file_size == cur_size)
    printf("File Successfully Received! : %d\n", cur_size);
  else
    printf("Failed Received... : %d\n", cur_size);
  
  printf("Working Time : %lf sec\n", (double)(end - start)/CLOCKS_PER_SEC);
  
  return 0;
}

void error_handling(char *msg) {
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}
