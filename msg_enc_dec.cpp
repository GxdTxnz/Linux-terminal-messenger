#include <vector>
#include <string>
#include <iostream>


void enc_msg(const char * input, char * output, size_t io_buffer_length)
{
  bool is_private = 0;
  
  for(size_t i = 0; i < 100; ++i)
  {
    if (input[0] == '@' && input[i] == ':')
    {
      is_private = 1;
      break;
    }
  }

  if(is_private)
  {
    size_t i = 0;
    size_t j = 0;

    bool name_end_flag = 0;
    bool name_start_flag = 0;
    bool at_last_one_name_exist = 0;

    while(input[i] != ':')
    {
      if (((input[i] >= '0' && input[i] <= '9') || (input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')) && !name_end_flag)
      {
        if (name_start_flag && at_last_one_name_exist)
        {
          output[j] = ',';
          ++j;
        }
      output[j] = input[i];
      ++j;

      name_start_flag = 0;
      at_last_one_name_exist = 1;

      } 
      
      else if (input[i] == '@') 
      {
        name_end_flag = 0;
        name_start_flag = 1;
      } 
      else 
      {
        name_end_flag = 1;
      }
      ++i;
    }

    for (; j < 100;)
    {
      output[j] = 0x00;
      ++j;
    }

    ++i;
    
    while (input[i] == ' ')
    {
      ++i;
    }
    
    if (at_last_one_name_exist)
    {
      for (auto c: "Private")
      {
        output[j] = c;
        ++j;
      }
      
      for (; j < 128;)
      {
        output[j] = 0x00;
        ++j;
      }
    } 

    else 
    {
      for (auto c: "Public")
      {
        output[j] = c;
        ++j;
      }
      
      for (; j < 128;)
      {
        output[j] = 0x00;
        ++j;
      }
    }
    
    for (j = 128; j < io_buffer_length; ++i)
    {
      if (input[i] == 0x00)
      {
        break;
      }
      output[j] = input[i];
      ++j;
    }

    for (; j < io_buffer_length;)
    {
      output[j] = 0x00;
      ++j;
    }
  }



  if (!is_private)
  {
    size_t j = 100;
    
    for (auto c: "Public")
    {
      output[j] = c;
      ++j;
    }
    
    for (; j < 128;)
    {
      output[j] = 0x00;
      ++j;
    }
    
    j = 128;
    
    for (size_t i = 0; i < io_buffer_length; ++i)
    {
      output[j] = input[i];
      ++j;
    }
  }
}


void create_enc_msg(const char * dest_name, const char * message_info, const std::string& message, size_t io_buffer_length, char * output)
{
  size_t j = 0;
  
  for (size_t i = 0; dest_name[i] != 0; ++i)
  {
    output[j] = dest_name[i];
    ++j;
  }

  for (; j < 100;)
  {
    output[j] = 0x00;
    ++j;
  }

  for (size_t i = 0; message_info[i] != 0; ++i)
  {
    output[j] = message_info[i];
    ++j;
  }

  for (; j < 128;)
  {
    output[j] = 0x00;
    ++j;
  }

  for (auto& c: message)
  {
    output[j] = c;
    ++j;
  }
  
  for (; j < io_buffer_length;)
  {
    output[j] = 0x00;
    ++j;
  }
}


void dec_msg(const char * encoded_message, size_t io_buffer_length, std::vector<std::string>& dest_names, std::string& message_info, std::string&  message)
{
  size_t name_char_count = 0;
  size_t name_char_firs_pos = 0;
  bool new_name_flag = 0;

  for (size_t i = 0; i < 100; ++i)
  {
    if (encoded_message[0] == 0x00)
    {
      break;
    }

    if ((encoded_message[i] == ',') || (encoded_message[i] == 0x00))
    {
      if (new_name_flag)
      {
        name_char_firs_pos += name_char_count + new_name_flag;
      }
      name_char_count = i - name_char_firs_pos;

      dest_names.push_back(std::string(encoded_message + name_char_firs_pos, name_char_count));

      if (encoded_message[i] == 0x00)
      {
        break;
      }

      if (encoded_message[i] == ',')
      {
        new_name_flag = 1;
        continue;
      }
    }
  }

  name_char_count = 0;
  
  for (size_t i = 100; i < 128; ++i)
  {
    if (encoded_message[100] == 0x00)
    {
      break;
    }

    if (encoded_message[i] == 0x00)
    {
      name_char_count = i - 100;

      message_info = std::string(encoded_message + 100, name_char_count);

      if (encoded_message[i] == 0x00)
      {
        break;
      }
    }
  }

  name_char_count = 0;
  for (size_t i = 128; i < io_buffer_length; ++i)
  {
    if (encoded_message[128] == 0x00)
    {
      break;
    }

    if (encoded_message[i] == 0x00)
    {
      name_char_count = i - 128;

      message = std::string(encoded_message + 128, name_char_count);

      if (encoded_message[i] == 0x00)
      {
        break;
      }
    }
  }
}

void print_enc_msg(const char * encoded_message, size_t io_buffer_length)
{
  std::cout << std::endl;
  
  for (size_t i = 0; i < io_buffer_length; ++i)
  {
    if (encoded_message[i] == 0x00)
    {
      std::cout << ".";
    }
    
    else
    {
      std::cout << encoded_message[i];
    }
  }
  std::cout << std::endl;
}

