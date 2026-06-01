#include "hex.h"


/* Array that asociates a number to it's hexadecimal representation. */
const char hex_table[16] = {'0', '1', '2', '3', '4',
	'5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};


/**
 * byte_print -	prints a byte in hexadecimal representation
 * @byte:		byte to print
 * Return:		void
 */
void byte_print(uint8_t byte)
{
	char hex_byte[2];

	hex_byte[1] = hex_table[(byte & 15)];
	hex_byte[0] = hex_table[((byte & 240) >> 4)];
	write(1, hex_byte, 2);
}


/**
 * hex_data -	prints len bytes starting at data in hexadecimal representation
 * @data:		pointer to the data to be printed
 * @len:		length of the data to be printed, amount of nodes
 * @data_size:	data type size in bytes
 * Return:		void
 */
void hex_data(void *data, unsigned int len, unsigned int data_size)
{
	unsigned int i = 0, j = 0, pos = 0;
	uint8_t *byte = data;

	if (!data)
		return;
	for (; i < len; i++)
	{
		for (j = data_size; j > 0; j--)
		{
			pos = (i * data_size) + j - 1;
			byte_print(byte[pos]);
		}
		write(1, " ", 1);
	}
}


/**
 * hex_dump -	prints len bytes starting at data in hexadecimal representation
 * @data:		pointer to the data to be printed
 * @len:		length of the data to be printed, amount of nodes
 * Return:		void
 */
void hex_dump(void *data, unsigned int len)
{
	unsigned int i = 0;
	uint8_t *byte = data;

	if (!data)
		return;
	for (; i < len; i++)
	{
		if (i)
			write(1, " ", 1);
		if (!(i % 8))
			write(1, "\n", 1);
		byte_print(byte[i]);
	}
}


/**
 * hex_str -	prints len bytes starting at data in hexadecimal representation
 * @data:		pointer to the data to be printed
 * @len:		length of the data to be printed, amount of nodes
 * Return:		void
 */
void hex_str(void *data, unsigned int len)
{
	unsigned int i = 0;
	uint8_t *byte = data;

	if (!data)
		return;
	for (; i < len; i++)
		byte_print(byte[i]);
}
