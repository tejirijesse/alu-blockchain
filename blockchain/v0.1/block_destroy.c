#include "blockchain.h"

/**
 * block_destroy - Deletes an existing block
 * @block: Block to delete
 */
void block_destroy(block_t *block)
{
	free(block);
}
