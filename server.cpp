#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <strings.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cerrno>
#include <cstring>

//using namespace std;

std::map<std::string, int> clients_list;
pthread_mutex_t clients_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_thread(void *);

const int MAX_CLIENTS = 100;
const int MAX_NAME_LENGTH = 50;
const int MAX_MESSAGE_LENGTH = 512;
const int PORT_NUMBER = 2025;

void *client_thread(void *arg)
{
  char client_name[MAX_NAME_LENGTH];
  bzero(client_name, sizeof(client_name));

  int fd = *(int *)arg;
  int rv;

  rv = recv(fd, client_name, sizeof(client_name), 0);
  
  if (rv == -1)
  {
    std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
    close(fd);
    pthread_exit(NULL);
  } 
  
  else if (rv == 0) 
  {
    close(fd);
    pthread_exit(NULL);
    
  }

  pthread_mutex_lock(&clients_list_mutex);
  clients_list[client_name] = fd;
  pthread_mutex_unlock(&clients_list_mutex);



}

int main(int argc, char* argv[])
{

}
