#include "cli.h"



/**
 * mine -	Mines a block.
 * @d:		Struct containing all data.
 * Return:	0 on succes, non zero otherwise.
 */
int mine(clid_t *d)
{
	block_t *block = NULL, *prev = NULL;
	int i = 0, size = 0;
	transaction_t *tx = NULL, *coinbase = NULL;
	llist_t *aux = NULL;

	if (!d)
		return (m_error(50));
	if (!d->bc)
		return (m_error(51));
	if (!d->bc->chain || !d->bc->unspent)
		return (m_error(53));
	if (!d->key)
		return (m_error(56));
	prev = llist_get_tail(d->bc->chain);
	block = block_create(prev, (int8_t *)"New Block", 9);
	block->info.difficulty = blockchain_difficulty(d->bc);
	coinbase = coinbase_create(d->key, block->info.index);
	llist_add_node(block->transactions, (llist_node_t)coinbase, ADD_NODE_FRONT);
	purge_tx(d->local_pool, d->bc->unspent);
	size = llist_size(d->local_pool);
	for (; i < size; i++)
	{
		tx = llist_get_node_at(d->local_pool, i);
		if (transaction_is_valid(tx, d->bc->unspent))
			llist_add_node(block->transactions, (llist_node_t)tx,
			ADD_NODE_REAR);
	}
	block_mine(block);
	if (block_is_valid(block, prev, d->bc->unspent))
	{
		block_destroy(block);
		return (m_error(54));
	}
	llist_add_node(d->bc->chain, (llist_node_t)block, ADD_NODE_REAR);
	aux = update_unspent(block->transactions, block->hash, d->bc->unspent);
	if (!aux)
		return (m_error(55));
	d->bc->unspent = aux;
	return (0);
}


/**
 * search_tx -	checks wether the current node is the one looked for
 *
 * @local_pool:	list of transactions
 *
 * @unspent:	list of unspent transactions
 *
 * Return:		an integer
 */
int search_tx(transaction_t *tx, llist_t *unspent)
{
	return (!transaction_is_valid(tx, unspent));
}


/**
 * purge_tx -	deletes invalid transactions from local pool
 *
 * @local_pool:	list of transactions
 *
 * @unspent:	list of unspent transactions
 *
 * Return:		void
 */
void purge_tx(llist_t *local_pool, llist_t *unspent)
{
	int sa = 0, sb = 0;

	do {
		sa = llist_size(local_pool);
		llist_remove_node(local_pool, (node_ident_t)search_tx,
			unspent, 1, (node_dtor_t)transaction_destroy);
		sb = llist_size(local_pool);
	} while (sb < sa);
}


/**
 * m_error -	Creates a transaction.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int m_error(int no)
{
	if (no == 50)
		printf("No struct with data specified.\n");
	if (no == 51)
		printf("No blockchain initialized.\n");
	if (no == 52)
		printf("Failed to create blockchain.\n");
	if (no == 53)
		printf("No chain or unspent list available in blockchain.\n");
	if (no == 54)
		printf("Couldn't create block.\n");
	if (no == 55)
		printf("Couldn't update unspent.\n");
	if (no == 56)
		printf("No key specified.\n");
	return (no);
}
