#include "cli.h"



/**
 * coin_dist -	Prints the distribution of coins.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int coin_dist(clid_t *d)
{
	dcoin_t *clist = NULL;

	if (!d)
		return (i_error(30));
	if (!d->bc)
		return (i_error(31));
	printf("<--- Coin Distribution --->\n");
	llist_for_each(d->bc->unspent, (node_func_t)compute_node, &clist);
	print_dcoin_list(clist);
	printf("<--- Coin Distribution End --->\n\n");
	free_dcoin_list(clist);
	return (0);
}


/**
 * compute_node -	Adds a new node or updates the amount of an existing one.
 * @node:			Node to gather the inormation from.
 * @idx:			Nodes index, not used.
 * @clist:			List where to save data in.
 * Return:			An integer.
 */
int compute_node(unspent_tx_out_t *node, int idx, dcoin_t **clist)
{
	dcoin_t *owner = NULL;

	(void)idx;
	owner = search_pos(*clist, node);
	if (!owner)
	{
		owner = malloc(sizeof(dcoin_t));
		if (!owner)
			return (1);
		owner->next = *clist;
		memcpy(owner->pub, node->out.pub, EC_PUB_LEN);
		owner->amount = 0;
		*clist = owner;
	}
	owner->amount += node->out.amount;
	return (0);
}


/**
 * search_pos -	Searchs the dcoin node position in the list.
 * @clist:		List.
 * @node:		Node.
 * Reutrn:		Corresponding node.
 */
dcoin_t *search_pos(dcoin_t *clist, unspent_tx_out_t *node)
{
	int value = 0;
	dcoin_t *aux = NULL;

	if (!clist)
		return (NULL);
	value = memcmp(clist->pub, node->out.pub, EC_PUB_LEN);
	if (value > 0)
		return (NULL);
	if (!value)
		return (clist);
	aux = search_pos(clist->next, node);
	if (!aux)
	{
		aux = malloc(sizeof(dcoin_t));
		if (!aux)
			return (NULL);
		aux->next = clist->next;
		clist->next = aux;
		memcpy(aux->pub, node->out.pub, EC_PUB_LEN);
		aux->amount = 0;
	}
	return (aux);
}


/**
 * free_dcoin_list -	Frees a list.
 * @clist:		List to be freed.
 * Return:		Void.
 */
void free_dcoin_list(dcoin_t *clist)
{
	if (!clist)
		return;
	free_dcoin_list(clist->next);
	free(clist);
}


/**
 * print_dcoin_list -	Prints the hash table data.
 * @clist:		List to be printed.
 * Return:		void
 */
void print_dcoin_list(dcoin_t *clist)
{
	dcoin_t *aux = clist;
	int total = 0;

	while (aux)
	{
		HEXS(aux->pub, EC_PUB_LEN);
		printf(": %u\n", aux->amount);
		total += aux->amount;
		aux = aux->next;
	}
	printf("\nTotal amount of coins: %u\n", total);
}
