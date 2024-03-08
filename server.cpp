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
std::mutex clients_mutex;
pthread_mutex_t clients_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *);

const int MAX_CLIENTS = 100;
const int MAX_NAME_LENGTH = 50;
const int MAX_MESSAGE_LENGTH = 512;
const int PORT_NUMBER = 2025;


void *client_thread(void *client_socket_fd) // Функция обработки потока клиента
{
  char client_name[MAX_NAME_LENGTH];
  char encd_msg[MAX_MESSAGE_LENGTH];
  char output[MAX_MESSAGE_LENGTH];
  
  std::vector<std::string> dest_names;
  std::string msg_info;
  std::string msg;

  int fd = *(int *)client_socket_fd;

  recv(fd, client_name, sizeof(client_name), 0); // Получаем имя клиента

  // Отправляем сообщение о входе в чат для всех клиентов
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    for (const auto &client : clients_list) 
    {
      create_enc_msg(client_name, "Server:online", " is online", sizeof(output), output);
      send(client.second, output, sizeof(output), 0);
    }
  }
// Отправляем список активных клиентов новому клиенту
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
// Добавляем клиента в список
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients_list[std::string(client_name)] = fd;
  }

  // Выводим информацию о клиенте
  std::cout << "Thread ID: " << pthread_self() << " socket_fd: " << fd << " Name: " << client_name << std::endl;

  // Основной цикл обработки сообщений клиента
  while (true) 
  {
    bzero(encd_msg, sizeof(encd_msg));
    dest_names.clear();
    msg_info.clear();
    msg.clear();
    bzero(output, sizeof(output));

    int rv = recv(fd, encd_msg, sizeof(encd_msg), 0); // Получаем сообщение от клиента
    
    if (rv == -1) 
    {
      std::cerr << "Error reading from client: " << std::strerror(errno) << std::endl;
      break;
    } 
    
    else if (rv == 0) 
    {
       break;
    }

    dec_msg(encd_msg, sizeof(encd_msg), dest_names, msg_info, msg); // Декодируем сообщение

    if (!dest_names.empty()) // Обрабатываем сообщение
    {
      for (const auto &d_name : dest_names) // Приватное сообщение
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
    
    else // Публичное сообщение
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

  // Удаляем клиента из списка при отключении
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients_list.erase(client_name);
    
    for (const auto &client : clients_list) 
    {
      create_enc_msg(client_name, "Server:online", " left the chat", sizeof(output), output);
      send(client.second, output, sizeof(output), 0);
    }
  }
  
  // Закрываем соединение и завершаем поток
  std::cout << "Closing connection with socket_fd: " << fd << ".  Name: " << client_name << ".  Exiting the thread." << std::endl;
  
  close(fd);
  pthread_exit(nullptr);
}

int main(int argc, char *argv[])
{
  int master_socket_fd;
  struct sockaddr_in server_addr;
  const int opt = 1;
  pthread_t thread[MAX_CLIENTS];
  int client_socket_fd;

  master_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Создаем TCP сокет
  
  if (master_socket_fd < 0)
  {
    std::cerr << "Cannot open tcp socket" << std::endl;
    return 1;
  }

  if (setsockopt(master_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&opt, sizeof(opt)) == -1) // Устанавливаем опции сокета
  {
    std::cerr << "setsockopt" << std::endl;
    return 1;
  }

  // Настраиваем адрес сервера
  bzero((void *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT_NUMBER);

  if (bind(master_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) // Привязываем сокет к адресу
  {
    std::cerr << "Cannot bind to port " << PORT_NUMBER << std::endl;
    return 1;
  }

  if (listen(master_socket_fd, MAX_CLIENTS) == -1) // Ожидаем подключений клиентов
  {
    std::cerr << "Cannot listen" << std::endl;
    return 1;
  }

  std::cout << "Listening socket_fd " << master_socket_fd << " on port " << PORT_NUMBER << "..." << std::endl; // Выводим информацию о прослушиваемом порту

  while (true) // Бесконечный цикл обработки новых подключений
  {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero((void *)&client_addr, sizeof(client_addr));

    client_socket_fd = accept(master_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len); // Принимаем новое подключение
    
    if (client_socket_fd == -1)
    {
      std::cerr << "Cannot accept connection" << std::endl;
      continue;
    }
    else
    {
      std::cout << "Incoming connection from: " << inet_ntoa(client_addr.sin_addr) << " on socket_fd: " << client_socket_fd << std::endl;
    }

    pthread_create(&thread[client_socket_fd], nullptr, client_thread, reinterpret_cast<void *>(&client_socket_fd)); // Создаем новый поток для обработки подключения
  }

    return 0;
}
