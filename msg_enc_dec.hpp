#ifndef MSG_ENC_DEC_HPP
#define MSG_ENC_DEC_HPP

#include <vector>
#include <string>

void enc_msg(const char *input, char *output, size_t io_buffer_length);

void dec_msg(const char *encd_msg, size_t io_buffer_length, std::vector<std::string> &dest_names, std::string &msg_info, std::string &msg);

void create_enc_msg(const char *dest_name, const char *msg_info, const std::string &msg, size_t io_buffer_length, char *output);

void print_enc_msg(const char *encd_msg, size_t io_buffer_length);

#endif

