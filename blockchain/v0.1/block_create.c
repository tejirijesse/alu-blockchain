#include "blockchain.h"

/**
 * block_create - Creates a block structure
 * @prev: Previous Block in the Blockchain
 * @data: Memory area to duplicate in the Block's data
 * @data_len: Number of bytes to duplicate in data
 *
 * Return: Pointer to the allocated block, or NULL upon failure
 */
block_t *block_create(block_t const *prev, int8_t const *data,
	uint32_t data_len)
{
	block_t *block;

	if (!prev || (!data && data_len))
		return (NULL);
	if (data_len > BLOCKCHAIN_DATA_MAX)
		data_len = BLOCKCHAIN_DATA_MAX;

	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	block->info.index = prev->info.index + 1;
	block->info.timestamp = time(NULL);
	memcpy(block->info.prev_hash, prev->hash, SHA256_DIGEST_LENGTH);
	if (data_len)
		memcpy(block->data.buffer, data, data_len);
	block->data.len = data_len;

	return (block);
}
