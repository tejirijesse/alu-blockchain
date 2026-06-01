#include "cli.h"



/**
 * load_blockchain -	Loads a blockchain.
 * @d:					Struct containing all data.
 * Return:				0 on succes, non zero otherwise.
 */
int load_blockchain(clid_t *d)
{
	if (!d)
		return (lsb_error(20));
	if (!d->av[1])
		return (lsb_error(21));
	d->bc = blockchain_deserialize(d->av[1]);
	if (!d->bc)
		return (lsb_error(22));
	return (0);
}


/**
 * save_blockchain -	Saves a blockchain.
 * @d:					Struct containing all data.
 * Return:				0 on succes, non zero otherwise.
 */
int save_blockchain(clid_t *d)
{
	if (!d)
		return (lsb_error(20));
	if (!d->av[1])
		return (lsb_error(21));
	if (!d->bc)
		return (lsb_error(24));
	if (blockchain_serialize(d->bc, d->av[1]))
		return (lsb_error(23));
	return (0);
}


/**
 * lsb_error -	Handles errors.
 * @no:			Error index.
 * Return:		Number that represents the error.
 */
int lsb_error(int no)
{
	if (no == 20)
		printf("No struct with data specified.\n");
	else if (no == 21)
		printf("No path specified.\n");
	else if (no == 22)
		printf("Couldn't deserialize blockchain. Check if path is valid.\n");
	else if (no == 23)
		printf("Couldn't serialize blockchain. Check if path is valid.\n");
	else if (no == 24)
		printf("No blockchain initialized.\n");
	return (no);
}
