#include "blockchain.h"

/**
 * block_hash - Computes the hash of a block
 * @block: Block to hash
 * @hash_buf: Buffer to store the hash
 *
 * Return: Pointer to hash_buf, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t len;

	if (!block || !hash_buf || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (NULL);

	len = sizeof(block->info) + block->data.len;
	return (sha256((int8_t const *)block, len, hash_buf));
}
