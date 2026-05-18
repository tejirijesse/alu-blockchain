#include "blockchain.h"

/**
 * blockchain_create - Creates a Blockchain structure
 *
 * Return: Pointer to the created blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	static uint8_t const hash[SHA256_DIGEST_LENGTH] = {
		0xc5, 0x2c, 0x26, 0xc8, 0xb5, 0x46, 0x16, 0x39,
		0x63, 0x5d, 0x8e, 0xdf, 0x2a, 0x97, 0xd4, 0x8d,
		0x0c, 0x8e, 0x00, 0x09, 0xc8, 0x17, 0xf2, 0xb1,
		0xd3, 0xd7, 0xff, 0x2f, 0x04, 0x51, 0x58, 0x03
	};
	blockchain_t *blockchain;
	block_t *genesis_block;

	genesis_block = calloc(1, sizeof(*genesis_block));
	blockchain = calloc(1, sizeof(*blockchain));
	if (!genesis_block || !blockchain)
	{
		free(genesis_block);
		free(blockchain);
		return (NULL);
	}

	genesis_block->info.timestamp = 1537578000;
	memcpy(genesis_block->data.buffer, "Holberton School", 16);
	genesis_block->data.len = 16;
	memcpy(genesis_block->hash, hash, SHA256_DIGEST_LENGTH);

	blockchain->chain = llist_create(MT_SUPPORT_TRUE);
	if (!blockchain->chain)
	{
		free(genesis_block);
		free(blockchain);
		return (NULL);
	}
	if (llist_add_node(blockchain->chain, genesis_block, ADD_NODE_FRONT) != 0)
	{
		llist_destroy(blockchain->chain, 1, NULL);
		free(genesis_block);
		free(blockchain);
		return (NULL);
	}

	return (blockchain);
}
