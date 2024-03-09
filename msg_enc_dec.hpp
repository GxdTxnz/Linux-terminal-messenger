#include <string>
#include <vector>

void enc_msg(const char * input, char * output, size_t io_buffer_length);

void dec_msg(const char * encoded_message, size_t io_buffer_length, std::vector<std::string>& dest_names, std::string& message_info, std::string&  message);

void create_enc_msg(const char * dest_name, const char * message_info, const std::string& message, size_t io_buffer_length, char * output);

void print_enc_msg(const char * encoded_message, size_t io_buffer_length);

