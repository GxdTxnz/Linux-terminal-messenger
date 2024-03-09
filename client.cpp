#include <iostream>
#include <vector>
#include <string>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "msg_enc_dec.hpp"
#include "init_terminal.hpp"

bool terminal_color = true;

void *sendMessage(void *arg);
void *receiveMessage(void *arg);

int main(int argc, char *argv[]) 
{
  if (argc < 2) 
	{
    std::cerr << "Syntax: ./client <server_ip_address>" << std::endl;
    return 1;
  }

  const char *ip_address = argv[1];
  const int port_number = 2025;
  int socket_desc;
  struct sockaddr_in server_addr;

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) 
	{
    std::cerr << "Cannot open socket with port number " << port_number << std::endl;
    return 1;
  }

  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_number);

    
  if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) 
	{
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return 1;
  }

    
  if (connect(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
	{
    std::cerr << "Cannot connect to the server with IP " << ip_address << std::endl;
    return 1;
  }

  pthread_t send_thread_id;
  pthread_t recv_thread_id;

  pthread_create(&send_thread_id, NULL, sendMessage, (void *) &socket_desc);
  pthread_create(&recv_thread_id, NULL, receiveMessage, (void *) &socket_desc);

  pthread_join(recv_thread_id, NULL);

  close(socket_desc);
  return 0;
}

void *sendMessage(void *arg) 
{
  int socket_desc = *(int *) arg;
  char client_name[50];

  std::vector<std::string> dest_names;
  std::string message_info;
  std::string message;

  char input[392];
  char input_encoded[512];

  bzero(client_name, sizeof(client_name));
  std::cout << "Write your name: " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
  std::cin.getline(client_name, 50);
  std::cout << terminal::RESET_ALL;

  send(socket_desc, client_name, sizeof(client_name), 0);

  std::cout << "Private message pattern: @<Destination name>: <message>" << std::endl;
  std::cout << "Public message typed as is: <message>" << std::endl;
  std::cout << "Write your first message !!!" << std::endl << std::endl;

  while (1) 
	{
    bzero(input, sizeof(input));
    bzero(input_encoded, sizeof(input_encoded));
    dest_names.clear();
    message_info.clear();
    message.clear();

    std::cin.getline(input, sizeof(input));

    int input_size = std::cin.gcount();
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int erase_line_count = 0;
        
		if ((input_size % w.ws_col) != 0) 
		{
      erase_line_count = (int) (input_size / w.ws_col) + 1;
    } 
		
    else 
		{
      erase_line_count = (int) (input_size / w.ws_col);
    }
        
		while (erase_line_count-- != 0) 
		{
      std::cout << terminal::Cursor_Previous_Line() << terminal::Erase_Line();
    }

    if (input_size == 1) 
		{
      continue;
    }

    enc_msg(input, input_encoded, sizeof(input_encoded));
    send(socket_desc, input_encoded, sizeof(input_encoded), 0);
    dec_msg(input_encoded, sizeof(input_encoded), dest_names, message_info, message);

    if (!dest_names.empty()) 
		{
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_BLUE;
            
			for (auto it = dest_names.begin(); it != dest_names.end(); ++it) 
			{
        std::cout << *it;
                
				if (*it != dest_names.back()) 
				{
          std::cout << ", ";
        }
      }
      std::cout << terminal::RESET_ALL << "<- " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_YELLOW << message << terminal::RESET_ALL << std::endl;
    } 
		else 
		{
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_YELLOW << message << terminal::RESET_ALL << std::endl;
    }
  }
}

void *receiveMessage(void *arg) 
{
  char input[512];
  std::vector<std::string> names_from;
  std::string message_info;
  std::string message_from;

  int socket_desc = *(int *) arg;

  while (1) 
	{
    names_from.clear();
    message_info.clear();
    message_from.clear();

    int rv = recv(socket_desc, input, sizeof(input), 0);
        
		if (rv == -1) 
		{
      std::cerr << "Error reading from server: " << strerror(errno) << std::endl;
      break;
    } 
		
		else if (rv == 0) 
		{
      break;
    }

    dec_msg(input, sizeof(input), names_from, message_info, message_from);

    if (!message_info.compare("Private")) 
		{
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_RED << names_from.front() << terminal::RESET_ALL << "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN << message_from << terminal::RESET_ALL << std::endl;
    } 
		
		else if (!message_info.compare("Public")) 
		{
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE << names_from.front() << terminal::RESET_ALL << "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN << message_from << terminal::RESET_ALL << std::endl;
    } 
		
		else if (!message_info.compare("Server:online")) 
		{
      std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
            
			for (auto it = names_from.begin(); it != names_from.end(); ++it) 
			{
        std::cout << *it;
                
				if (*it != names_from.back()) 
				{
          std::cout << ", ";
        }
      }
      std::cout << terminal::RESET_ALL << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE << message_from << terminal::RESET_ALL << std::endl;
    }
  }

  std::cout << "Server is down!!! Closing receiveMessage and socket..." << std::endl;
  close(socket_desc);
  pthread_exit(NULL);
}

