#include "blockchain.h"

/**
 * blockchain_destroy - Deletes an existing Blockchain
 * @blockchain: Blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	block_t *block;

	if (!blockchain)
		return;
	if (!blockchain->chain)
	{
		free(blockchain);
		return;
	}

	block = llist_pop(blockchain->chain);
	while (block)
	{
		block_destroy(block);
		block = llist_pop(blockchain->chain);
	}
	llist_destroy(blockchain->chain, 0, NULL);
	free(blockchain);
}
