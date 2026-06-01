#include "cli.h"



/**
 * print_block -	Prints a block.
 * @d:				Struct containing all data.
 * Return:			0 on succes, non zero otherwise.
 */
int print_block(clid_t *d)
{
	int idx = 0;
	block_t *b = NULL;
	int (*func)(block_t *, int, void *) = NULL;

	if (!d)
		return (i_error(30));
	if (!d->bc)
		return (i_error(31));
	idx = atoi(d->av[1]);
	b = llist_get_node_at(d->bc->chain, idx);
	if (!b)
		return (i_error(32));
	func = (d->p_mode) ? pc_block : pb_block;
	func(b, 0, NULL);
	printf("\n");
	return (0);
}


/**
 * bc_dist -	Prints the blockchain.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int bc_dist(clid_t *d)
{
	int (*func)(block_t *, int, void *) = NULL;

	if (!d)
		return (i_error(30));
	if (!d->bc)
		return (i_error(31));
	func = (d->p_mode) ? pc_block : pb_block;
	printf("<--- Blockchain Begin --->\n");
	llist_for_each(d->bc->chain, (node_func_t)func, NULL);
	printf("<--- Blockchain End --->\n\n");
	return (0);
}


/**
 * pb_block -	Print a block briefly.
 * @b:			Block to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pb_block(block_t *b, int idx, void *arg)
{
	(void)idx;
	(void)arg;
	printf("<--- Block Begin --->\n");
	printf("Index: %d\n", b->info.index);
	printf("Hash: ");
	fflush(NULL);
	HEXS(b->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Buffer: (%d) %s\n", b->data.len, b->data.buffer);
	printf("Transactions: (%d)\n", llist_size(b->transactions));
	llist_for_each(b->transactions, (node_func_t)pb_tx, NULL);
	printf("<--- Block End --->\n\n");
	return (0);
}


/**
 * pc_block -	Print a block completely.
 * @b:			Block to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pc_block(block_t *b, int idx, void *arg)
{
	(void)idx;
	(void)arg;
	printf("<--- Block Begin --->\n");
	printf("Block Info:\n");
	printf("Index: %d\n", b->info.index);
	printf("Difficulty: %d\n", b->info.difficulty);
	printf("Timestamp: %ld\n", b->info.timestamp);
	printf("Nonce: %ld\n", b->info.nonce);
	printf("Previous Hash:");
	fflush(NULL);
	HEX(b->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Block Data:\n");
	printf("Buffer: %s\n", b->data.buffer);
	printf("Buffer length: %d\n", b->data.len);
	printf("Hash: ");
	fflush(NULL);
	HEX(b->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Transactions: (%d)\n", llist_size(b->transactions));
	llist_for_each(b->transactions, (node_func_t)pc_tx, NULL);
	printf("<--- Block End --->\n\n");
	return (0);
}
