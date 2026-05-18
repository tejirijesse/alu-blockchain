#include "blockchain.h"

/**
 * block_is_genesis - Checks if a block is the Genesis Block
 * @block: Block to check
 *
 * Return: 0 if valid, 1 otherwise
 */
static int block_is_genesis(block_t const *block)
{
	blockchain_t *blockchain;
	block_t *genesis;
	int ret;

	blockchain = blockchain_create();
	if (!blockchain)
		return (1);

	genesis = llist_get_head(blockchain->chain);
	ret = memcmp(block, genesis, sizeof(*genesis));
	blockchain_destroy(blockchain);
	return (ret != 0);
}

/**
 * block_is_valid - Checks if this and previous block are valid
 * @block: Block to check
 * @prev_block: Previous block, or NULL for the Genesis Block
 *
 * Return: 0 if valid, 1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

	if (!block || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (1);
	if (!prev_block && block->info.index != 0)
		return (1);
	if (block->info.index == 0)
		return (block_is_genesis(block));
	if (!prev_block)
		return (1);
	if (block->info.index != prev_block->info.index + 1)
		return (1);
	if (!block_hash(prev_block, hash_buf))
		return (1);
	if (memcmp(hash_buf, prev_block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);
	if (memcmp(hash_buf, block->info.prev_hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);
	if (!block_hash(block, hash_buf))
		return (1);
	if (memcmp(hash_buf, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (1);

	return (0);
}
