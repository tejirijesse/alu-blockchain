#include "cli.h"



/**
 * info -	Prints basic info about the blockchain.
 * @d:		Struct containing all data.
 * Return:	0 on succes, non zero otherwise.
 */
int info(clid_t *d)
{
	int no_blocks = 0, no_unspent = 0, no_tx = 0;

	if (!d)
		return (i_error(30));
	if (d->bc)
	{
		if (d->bc->chain)
			no_blocks = llist_size(d->bc->chain);
		if (d->bc->unspent)
			no_unspent = llist_size(d->bc->unspent);
	}
	if (d->local_pool)
		no_tx = llist_size(d->local_pool);
	printf("No. of blocks:\t\t\t\t%d\n", no_blocks);
	printf("No. of unspent transaction output:\t%d\n", no_unspent);
	printf("No. of pennding transactions:\t\t%d\n", no_tx);
	return (0);
}


/**
 * i_error -	Manages info errors.
 * @no:			Error index.
 * Return:		Number that represents the error.
 */
int i_error(int no)
{
	if (no == 30)
		printf("No struct with data specified.\n");
	if (no == 31)
		printf("No blockchain initialized.\n");
	if (no == 32)
		printf("No node at the specified index.\n");
	if (no == 33)
		printf("No local pool.\n");
	if (no == 34)
		printf("No unspent list available.\n");
	return (no);
}
