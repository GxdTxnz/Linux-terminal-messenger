#include "msg_enc_dec.hpp"
#include <array>
#include <iostream>

const size_t MAX_DEST_NAME_LENGTH = 100;
const size_t MAX_MESSAGE_INFO_LENGTH = 28;
const size_t DEST_NAME_OFFSET = 0;
const size_t MESSAGE_INFO_OFFSET = 100;
const size_t MESSAGE_OFFSET = 128;

void enc_msg(const char *input, char *output, size_t io_buffer_length)
{
  bool is_private = false;
  
  for (size_t i = 0; i < 100; ++i)
  {
    
    if (input[0] == '@' && input[i] == ':')
    {
      is_private = true;
      break;
    }
  }

  if (is_private)
  {
   //size_t j = 0;
    std::string dest_names;
    
    for (size_t i = 0; i < io_buffer_length; ++i)
    {
      if (input[i] == ':') break;
      
      if (isalnum(input[i]) || input[i] == '@')
      {
        dest_names += input[i];
      }
    }
    
    dest_names.resize(MAX_DEST_NAME_LENGTH, '\0');
    std::copy(dest_names.begin(), dest_names.end(), output);

    std::string msg_info = (dest_names.empty()) ? "Public" : "Private";
    msg_info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
    std::copy(msg_info.begin(), msg_info.end(), output + MESSAGE_INFO_OFFSET);

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
    std::fill(output + pos, output + io_buffer_length, '\0');
  }
  
  else 
  {
    std::string msg_info = "Public";
    msg_info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
    std::copy(msg_info.begin(), msg_info.end(), output + MESSAGE_INFO_OFFSET);
    std::copy(input, input + io_buffer_length, output + MESSAGE_OFFSET);
    std::fill(output + io_buffer_length, output + io_buffer_length, '\0');
  }
}

void create_enc_msg(const char *dest_name, const char *msg_info, const std::string &msg, size_t io_buffer_length, char *output) 
{
  std::string dest_names(dest_name);
  dest_names.resize(MAX_DEST_NAME_LENGTH, '\0');
  std::copy(dest_names.begin(), dest_names.end(), output);

  std::string info(msg_info);
  info.resize(MAX_MESSAGE_INFO_LENGTH, '\0');
  std::copy(info.begin(), info.end(), output + MESSAGE_INFO_OFFSET);

  std::copy(msg.begin(), msg.end(), output + MESSAGE_OFFSET);
  std::fill(output + MESSAGE_OFFSET + msg.size(), output + io_buffer_length, '\0');
}

void dec_msg(const char *encd_msg, size_t io_buffer_length, std::vector<std::string> &dest_names, std::string &msg_info, std::string &msg)
{
  size_t pos = 0;
  
  while (pos < MAX_DEST_NAME_LENGTH && encd_msg[pos] != '\0')
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

  msg_info = std::string(encd_msg + MESSAGE_INFO_OFFSET, MAX_MESSAGE_INFO_LENGTH);
  msg = std::string(encd_msg + MESSAGE_OFFSET, io_buffer_length - MESSAGE_OFFSET);
}

void print_enc_msg(const char *encd_msg, size_t io_buffer_length) 
{
  
  for (size_t i = 0; i < io_buffer_length; ++i) 
  {
    std::cout << ((encd_msg[i] == '\0') ? '.' : encd_msg[i]);
  }
  
  std::cout << std::endl;
}

