// *** Author: Zhiyi Zhang for CS118, Time: 04/10/2020, TCP Server Sample Code ***
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <fstream>

const int FILE_START_IND = 5;
const std::string HTML_TEXT_CONTENT_TYPE = "text/html";
const std::string PLAIN_TEXT_CONTENT_TYPE = "text/plain; charset=UTF-8";
const std::string PNG_IMAGE_CONTENT_TYPE = "image/png";
const std::string JPG_IMAGE_CONTENT_TYPE = "image/jpg";
const std::string BINARY_FILE_CONTENT_TYPE = "application/octet-stream";

std::string GetFileName(const char* recv_buffer_c) {
 std::string recv_buffer = recv_buffer_c;
 int file_name_length = recv_buffer.find(' ', FILE_START_IND) - FILE_START_IND;
 return recv_buffer.substr(FILE_START_IND, file_name_length);
}

std::string GetFileType(std::string file_name) {
 int file_type_start_ind = file_name.find('.') + 1;
 // If file_type_start_ind is 0, that means no dot is found;
 if (file_type_start_ind == 0) {
  return "";
 }
 return file_name.substr(file_type_start_ind, file_name.size() - file_type_start_ind);
}


int main()
{
 // *** Initialize socket for listening ***
 int sockfd;
 if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
   perror("socket");
   exit(1);
 }

 // *** Initialize local listening socket address ***
 struct sockaddr_in my_addr;
 memset(&my_addr, 0, sizeof(my_addr));
 my_addr.sin_family = AF_INET;
 my_addr.sin_port = htons(5678);
 my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY allows to connect to any one of the host’s IP address

 // *** Socket Bind ***
 if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
   perror("bind");
   exit(1);
 }

 // *** Socket Listen ***
 if (listen(sockfd, 10) == -1) {
   perror("listen");
   exit(1);
 }

 // *** Accept and Read&Write **
 int client_fd;
 struct sockaddr_in client_addr; // client address
 unsigned int sin_size;
 char recv_buf[1024];
 int recv_buf_size;
 while (1) { // main accept() loop
   sin_size = sizeof(struct sockaddr_in);
   if ((client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
     perror("accept");
     continue;
   }
   printf("server: got connection from %s\n", inet_ntoa(client_addr.sin_addr));
   memset(recv_buf, 0, 1024);
   while ((recv_buf_size = read(client_fd, recv_buf, 2024)) > 0)  {
     printf("String received from and resent to the client: %.*s", recv_buf_size, recv_buf);
     if (memcmp(recv_buf, "bye", strlen("bye")) == 0) {
       printf("Will close the connection\n");
       close(client_fd);
       exit(0);
     }
     std::string file_name = GetFileName(recv_buf);
     std::string file_type = GetFileType(file_name);

     std::ifstream requested_file(file_name);
     std::string file_contents((std::istreambuf_iterator<char>(requested_file)),
                         std::istreambuf_iterator<char>());

     std::string content_type = "";
     if (file_type == "html")
      content_type = HTML_TEXT_CONTENT_TYPE;
     else if (file_type == "txt")
      content_type = PLAIN_TEXT_CONTENT_TYPE;
     else if (file_type == "jpg")
      content_type = JPG_IMAGE_CONTENT_TYPE;
     else if (file_type == "png")
      content_type = PNG_IMAGE_CONTENT_TYPE;
     else
      content_type = BINARY_FILE_CONTENT_TYPE;

     std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " +
                            std::to_string(file_contents.size()) +
                            "\r\nContent-Type: " + content_type +
                            "\r\n\r\n" + file_contents;
     std::cout << response;

     write(client_fd, response.c_str(), response.size());
   }
   if (recv_buf_size < 0) {
     close(client_fd);
   }
 }
}

