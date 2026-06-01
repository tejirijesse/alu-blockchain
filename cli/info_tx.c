#include "cli.h"



/**
 * print_lp -	Prints the local pool of transactions.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int print_lp(clid_t *d)
{
	int (*func)(transaction_t *, int, void *) = NULL;

	if (!d)
		return (i_error(30));
	if (!d->local_pool)
		return (i_error(33));
	if (!llist_size(d->local_pool))
		printf("Local pool is empty.\n");
	else
	{
		func = (d->p_mode) ? pc_tx : pb_tx;
		printf("<--- Local Pool List --->\n");
		llist_for_each(d->local_pool, (node_func_t)func, NULL);
		printf("<--- Local Pool List End --->\n\n");
	}
	return (0);
}


/**
 * pb_tx -		Print a transaction briefly.
 * @tx:			Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pb_tx(transaction_t *tx, int idx, void *arg)
{
	int in = 0, out = 0;

	(void)idx;
	(void)arg;
	in = llist_size(tx->inputs);
	out = llist_size(tx->outputs);
	printf("Tx: (In: %d) (Out: %d)\t\t", in, out);
	fflush(NULL);
	HEXS(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}


/**
 * pc_tx -		Print a transaction completely.
 * @tx:			Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pc_tx(transaction_t *tx, int idx, void *arg)
{
	(void)idx;
	(void)arg;
	printf("--> Tx:");
	fflush(NULL);
	HEX(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("--> In: (%d)\n", llist_size(tx->inputs));
	llist_for_each(tx->inputs, (node_func_t)pc_in, NULL);
	printf("--> Out: (%d)\n", llist_size(tx->outputs));
	llist_for_each(tx->outputs, (node_func_t)pc_out, NULL);
	return (0);
}


/**
 * pc_in -		Print an in transaction completely.
 * @in:			Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pc_in(tx_in_t *in, int idx, void *arg)
{
	(void)idx;
	(void)arg;
	printf("Block Hash:");
	fflush(NULL);
	HEX(in->block_hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Tx Id:");
	fflush(NULL);
	HEX(in->tx_id, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Tx Out Hash:");
	fflush(NULL);
	HEX(in->tx_out_hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Signature:");
	fflush(NULL);
	HEX(in->sig.sig, in->sig.len);
	printf("\n");
	printf("---\n");
	return (0);
}


 /**
 * pc_out -		Print an out transaction completely.
 * @out:		Transaction to print.
 * @idx:		Node index, not used.
 * @arg:		NULL
 * Return:		Int.
 */
int pc_out(tx_out_t *out, int idx, void *arg)
{
	(void)idx;
	(void)arg;
	printf("Amount: %d\n", out->amount);
	printf("Public Key:");
	fflush(NULL);
	HEX(out->pub, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("Hash:");
	fflush(NULL);
	HEX(out->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	printf("---\n");
	return (0);
}
