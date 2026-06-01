#ifndef HEX_H
#define HEX_H


#include <stdint.h>
#include <unistd.h>



#define \
HEX(data, len) hex_dump(data, len)

#define \
HEXD(data) hex_data(data, (sizeof(data) / sizeof(*data)), sizeof(*data))

#define \
HEXDD(data, len) hex_data(data, len, sizeof(*data))

#define \
HEXS(data, len) hex_str(data, len)



void byte_print(uint8_t byte);
void hex_data(void *data, unsigned int len, unsigned int data_size);
void hex_dump(void *data, unsigned int len);
void hex_str(void *data, unsigned int len);


#endif
