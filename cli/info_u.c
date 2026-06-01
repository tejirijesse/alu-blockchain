#include "cli.h"



/**
 * print_unspent -	Prints unspent list.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int print_unspent(clid_t *d)
{
	int (*func)(unspent_tx_out_t *, int, void *) = NULL;

	if (!d)
		return (i_error(30));
	if (!d->bc)
		return (i_error(31));
	if (!d->bc->unspent)
		return (i_error(34));
	if (!llist_size(d->bc->unspent))
		printf("Empty unspent list.\n");
	else
	{
		func = (d->p_mode) ? pc_uto : pb_uto;
		printf("<--- Unspent Transaction List --->\n");
		llist_for_each(d->bc->unspent, (node_func_t)func, NULL);
		printf("<--- Unspent Transaction List End --->\n");
	}
	return (0);
}


/**
 * pb_uto -		Print an unspent transaction briefly.
 * @uto:		Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pb_uto(unspent_tx_out_t *uto, int idx, void *arg)
{
	(void)arg;
	(void)idx;
	printf("Hash: ");
	fflush(NULL);
	HEXS(uto->block_hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}


/**
 * pc_uto -		Print an unspent transaction completely.
 * @uto:		Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pc_uto(unspent_tx_out_t *uto, int idx, void *arg)
{
	(void)arg;
	(void)idx;
	printf("Hash: ");
	fflush(NULL);
	HEX(uto->block_hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Tx Id: ");
	fflush(NULL);
	HEX(uto->tx_id, SHA256_DIGEST_LENGTH);
	printf("\n");
	pc_out(&uto->out, 0, NULL);
	return (0);
}
