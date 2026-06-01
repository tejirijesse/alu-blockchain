#include "cli.h"



/**
 * main -		Command line interpreter.
 *
 * Return:		0 on success, non zero otherwise.
 */
int main(void)
{
	clid_t d;
	int (*func)(clid_t *);

	memset(&d, 0, sizeof(clid_t));
	while (!d.exit_cli)
	{
		get_input(d.av);
		if (d.av[0])
		{
			func = alpha_ret(d.av[0]);
			if (func)
				func(&d);
			free_args(d.av);
		}
	}
	free_data(&d);
	return (0);
}


/**
 * mine -	Initializes a blockchain.
 * @d:		Struct containing all data.
 * Return:	0 on succes, non zero otherwise.
 */
int bc_init(clid_t *d)
{
	if (!d)
		return (m_error(50));
	d->bc = blockchain_create();
	d->local_pool = llist_create(MT_SUPPORT_FALSE);
	if (!d->bc)
		return (m_error(52));
	printf("Blockchain initialized.\n");
	return (0);
}


/**
 * free_data -	Frees all data inside the struct.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int free_data(clid_t *d)
{
	if (!d)
		return (1);
	if (d->av[0])
		free_args(d->av);
	if (d->key)
		EC_KEY_free(d->key);
	if (d->bc)
		blockchain_destroy(d->bc);
	if (d->local_pool)
		llist_destroy(d->local_pool, 1, (node_dtor_t)transaction_destroy);
	memset(d, 0, sizeof(clid_t));
	printf("All data set to 0.\n");
	return (0);
}
