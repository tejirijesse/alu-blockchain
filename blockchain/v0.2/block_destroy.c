#include "blockchain.h"

/**
* block_destroy - deletes an existing Block.
*
* @block: pointer to the Block to delete.
*/
void block_destroy(block_t *block)
{
	free(block);
}

