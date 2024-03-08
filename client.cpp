#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <string>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <mutex>
#include <thread>

bool terminal_color = true;

volatile sig_atomic_t g_signal_received = 0;
std::mutex g_mutex;

void signal_handler(int signal);
void sendMessages(int socket_desc);
void receiveMessages(int socket_desc);

void signal_handler(int signal) //Обработчик сигнала завершения
{
  g_signal_received = signal;
}





int main(int argc, char* argv[])
{

}
