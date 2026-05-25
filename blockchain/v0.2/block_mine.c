#include "blockchain.h"

/**
 * block_mine - Mines a block with the required difficulty
 * @block: Block to mine
 */
void block_mine(block_t *block)
{
	if (!block)
		return;

	block->info.nonce = 0;
	block_hash(block, block->hash);
	while (!hash_matches_difficulty(block->hash, block->info.difficulty))
	{
		block->info.nonce++;
		block_hash(block, block->hash);
	}
}
