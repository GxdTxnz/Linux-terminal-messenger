#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cerrno>
#include <cstring>
#include <mutex>
#include "msg_enc_dec.hpp"

//using namespace std;

std::map<std::string, int> clients_list;
pthread_mutex_t clients_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *);

const int MAX_CLIENTS = 100;
const int MAX_NAME_LENGTH = 50;
const int MAX_MESSAGE_LENGTH = 512;
const int PORT_NUMBER = 2025;

void *client_thread(void *client_socket_fd)
{
  char client_name[MAX_NAME_LENGTH];
  char encd_msg[MAX_MESSAGE_LENGTH];
  char output[MAX_MESSAGE_LENGTH];
  
  std::vector<std::string> dest_names;
  std::string msg_info;
  std::string msg;

  int fd = *(int *)client_socket_fd;

  recv(fd, client_name, sizeof(client_name), 0);
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    for (const auto &client : clients_list) 
    {
      create_enc_msg(client_name, "Server:online", " is online", sizeof(output), output);
      send(client.second, output, sizeof(output), 0);
    }
  }

  if (!clients_list.empty()) 
  {
    std::string active_clients;
    size_t clients_count = 0;
    
    {
      std::lock_guard<std::mutex> lock(clients_mutex);
      
      for (const auto &client : clients_list) 
      {
        active_clients += client.first;
        ++clients_count;
        
        if (clients_count < clients_list.size()) 
        {
          active_clients += ",";
        }
      }
    }

    std::string is_are;

    if (clients_count > 1) 
    {
      is_are = " are online";
    } 
    else 
    {
      is_are = " is online";
    }

    create_enc_msg(active_clients.c_str(), "Server:online", is_are, sizeof(output), output);
    send(fd, output, sizeof(output), 0);
  }

  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients_list[std::string(client_name)] = fd;
  }

  std::cout << "Thread ID: " << pthread_self() << " socket_fd: " << fd << " Name: " << client_name << std::endl;

  while (true) 
  {
    bzero(encd_msg, sizeof(encd_msg));
    dest_names.clear();
    msg_info.clear();
    msg.clear();
    bzero(output, sizeof(output));

    int rv = recv(fd, encd_msg, sizeof(encd_msg), 0);
    
    if (rv == -1) 
    {
      std::cerr << "Error reading from client: " << std::strerror(errno) << std::endl;
      break;
    } 
    
    else if (rv == 0) 
    {
       break;
    }

    dec_msg(encd_msg, sizeof(encd_msg), dest_names, msg_info, msg);

    if (!dest_names.empty())
    {
      for (const auto &d_name : dest_names) 
      {
        int dest_socket_desc;
        {
          std::lock_guard<std::mutex> lock(clients_mutex);
          auto it = clients_list.find(d_name);
          
          if (it != clients_list.end()) 
          {
            dest_socket_desc = it->second;
          } 
          
          else 
          {
            std::cout << "From \"" << client_name << "\": destination name \"" << d_name << "\" not found" << std::endl;
            continue;
          }
        }

        std::cout << client_name << " -> " << d_name << ": " << msg << std::endl;

        create_enc_msg(client_name, "Private", msg, sizeof(output), output);
        send(dest_socket_desc, output, sizeof(output), 0);
      }
    } 
    
    else 
    {
      {
        std::lock_guard<std::mutex> lock(clients_mutex);
        
        for (const auto &client : clients_list) 
        {
          if (client.first.compare(client_name) == 0) 
          {
            continue;
          }
          std::cout << client_name << " -> " << client.first << ": " << msg << std::endl;

          create_enc_msg(client_name, "Public", msg, sizeof(output), output);
          send(client.second, output, sizeof(output), 0);
        }
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients_list.erase(client_name);
    
    for (const auto &client : clients_list) 
    {
      create_enc_msg(client_name, "Server:online", " left the chat", sizeof(output), output);
      send(client.second, output, sizeof(output), 0);
    }
  }
  
  std::cout << "Closing connection with socket_fd: " << fd << ".  Name: " << client_name << ".  Exiting the thread." << std::endl;
  
  close(fd);
  pthread_exit(nullptr);
}

int maint(int argc, char *argv[])
{

}
