#include "msg_enc_dec.hpp"
#include <array>
#include <iostream>

const size_t MAX_DEST_NAME_LENGTH = 100;
const size_t MAX_MESSAGE_INFO_LENGTH = 28;
const size_t DEST_NAME_OFFSET = 0;
const size_t MESSAGE_INFO_OFFSET = 100;
const size_t MESSAGE_OFFSET = 128;

/* Функция enc_msg кодирует сообщение в формате, понятном для передачи по сети.
 * Если сообщение адресовано конкретному получателю, оно считается приватным.
 * противном случае сообщение считается публичным.
 */

void enc_msg(const char *input, char *output, size_t io_buffer_length)
{
  bool is_private = false;
  
  for (size_t i = 0; i < 100; ++i) // Проверяем, адресовано ли сообщение конкретному получателю.
  {
    
    if (input[0] == '@' && input[i] == ':')
    {
      is_private = true;
      break;
    }
  }

  if (is_private) // Если сообщение приватное, кодируем его для конкретного получателя.
  {
   //size_t j = 0;
    std::string dest_names;
    
    for (size_t i = 0; i < io_buffer_length; ++i) // Извлекаем имена получателей из входной строки
    {
      if (input[i] == ':') break;
      
      if (isalnum(input[i]) || input[i] == '@')
      {
        dest_names += input[i];
      }
    }
    
    // Ограничиваем длину имен получателей и копируем в выходной буфер
    dest_names.resize(MAX_DEST_NAME_LENGTH, '\0');
    std::copy(dest_names.begin(), dest_names.end(), output);

    // Определяем тип сообщения (приватное или публичное) и копируем в выходной буфер
    std::string msg_info = (dest_names.empty()) ? "Public" : "Private";
    msg_info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
    std::copy(msg_info.begin(), msg_info.end(), output + MESSAGE_INFO_OFFSET);

    // Копируем само сообщение в выходной буфер, с учётом смещения
    size_t pos = io_buffer_length;
    
    for (size_t i = MESSAGE_OFFSET; i < io_buffer_length; ++i) 
    {
      if (input[i - MESSAGE_OFFSET] == '\0')
      {
        pos = i;
        break;
      }
      output[i] = input[i - MESSAGE_OFFSET];
    }
    std::fill(output + pos, output + io_buffer_length, '\0'); // Заполняем оставшуюся часть выходного буфера нулями
  }
  
  else // Если сообщение публичное, кодируем его для всех получателей
  {
    std::string msg_info = "Public"; // Определяем тип сообщения (публичное) и копируем в выходной буфер
    msg_info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
    std::copy(msg_info.begin(), msg_info.end(), output + MESSAGE_INFO_OFFSET);
    std::copy(input, input + io_buffer_length, output + MESSAGE_OFFSET); // Копируем само сообщение в выходной буфер, с учётом смещения
    std::fill(output + io_buffer_length, output + io_buffer_length, '\0'); // Заполняем оставшуюся часть выходного буфера нулями
  }
}

// Функция create_enc_msg создаёт закодированное сообщение с указанным адресатом, типом сообщения и содержанием
void create_enc_msg(const char *dest_name, const char *msg_info, const std::string &msg, size_t io_buffer_length, char *output) 
{
  std::string dest_names(dest_name); // Копируем имя адресата в выходной буфер с ограничением максимальной длины
  dest_names.resize(MAX_DEST_NAME_LENGTH, '\0');
  std::copy(dest_names.begin(), dest_names.end(), output);

  // Копируем информацию о сообщении (тип) в выходной буфер.
  std::string info(msg_info);
  info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
  std::copy(info.begin(), info.end(), output + MESSAGE_INFO_OFFSET);

  std::copy(msg.begin(), msg.end(), output + MESSAGE_OFFSET);  // Копируем содержимое сообщения в выходной буфер.
  std::fill(output + MESSAGE_OFFSET + msg.size(), output + io_buffer_length, '\0'); // Заполняем оставшуюся часть выходного буфера нулями.
}

// Функция dec_msg декодирует сообщение из переданного закодированного буфера.
void dec_msg(const char *encd_msg, size_t io_buffer_length, std::vector<std::string> &dest_names, std::string &msg_info, std::string &msg)
{
  size_t pos = 0;
  
  while (pos < MAX_DEST_NAME_LENGTH && encd_msg[pos] != '\0') // Извлекаем имена получателей из закодированного сообщения.
  {
    std::string name;
    
    while (pos < MAX_DEST_NAME_LENGTH && encd_msg[pos] != ',' && encd_msg[pos] != '\0') 
    {
      name += encd_msg[pos];
      ++pos;
    }
    dest_names.push_back(name);
    
    if (encd_msg[pos] == ',') ++pos;
  }

  msg_info = std::string(encd_msg + MESSAGE_INFO_OFFSET, MAX_MESSAGE_INFO_LENGTH); // Извлекаем информацию о типе сообщения
  msg = std::string(encd_msg + MESSAGE_OFFSET, io_buffer_length - MESSAGE_OFFSET); // Извлекаем содержимое сообщения
}

// Функция print_enc_msg выводит закодированное сообщение в консоль
void print_enc_msg(const char *encd_msg, size_t io_buffer_length) 
{
  
  for (size_t i = 0; i < io_buffer_length; ++i) 
  {
    std::cout << ((encd_msg[i] == '\0') ? '.' : encd_msg[i]);
  }
  
  std::cout << std::endl;
}

