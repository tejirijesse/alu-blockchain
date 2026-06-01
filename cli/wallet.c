#include "cli.h"



/**
 * load_wallet -	Loads a wallet from file wallet.sv in path.
 * @d:				Struct containing all data.
 * Return:			0 on succes, non zero otherwise.
 */
int load_wallet(clid_t *d)
{
	char ans = 0;

	if (!d)
		return (w_error(10));
	if (!d->av[1])
		return (w_error(11));
	d->key = ec_load(d->av[1]);
	if (d->key)
		printf("Key loaded.\n");
	else
	{
		printf("No key found in <%s>\n", d->av[1]);
		while (1)
		{
			printf("Do you want to create a key? <y> / <n>\n");
			read(0, &ans, 1);
			if ((ans != 'y') && (ans != 'n'))
				printf("No valid input.\n");
			if (ans == 'y')
			{
				return (create_wallet(d));
			}
			else
				break;
			while ((ans != 0) && (ans != '\n'))
				read(0, &ans, 1);
		}
	}
	return (0);
}


/**
 * create_wallet -	Creates a wallet.
 * @d:				Struct containing all data.
 * Return:			0 on succes, non zero otherwise.
 */
int create_wallet(clid_t *d)
{
	if (!d)
		return (w_error(10));
	d->key = ec_create();
	if (!d->key)
		return (w_error(12));
	printf("Key successfully created.\n");
	return (0);
}


/**
 * save_wallet -	Saves a wallet in file wallet.sv in path.
 * @d:				Struct containing all data.
 * Return:			0 on succes, non zero otherwise.
 */
int save_wallet(clid_t *d)
{
	if (!d)
		return (w_error(10));
	if (!d->av[1])
		return (w_error(11));
	if (!ec_save(d->key, d->av[1]))
		printf("Couldn't save the wallet.\n");
	else
		printf("Wallet saved.\n");
	return (0);
}


/**
 * w_error -	Handles errors.
 * @no:			Error index.
 * Return:		Number that represents the error.
 */
int w_error(int no)
{
	if (no == 10)
		printf("No struct with data specified.\n");
	else if (no == 11)
		printf("No path specified.\n");
	else if (no == 12)
		printf("Failed to create key.\n");
	else if (no == 13)
		printf("Failed to create key.\n");
	return (no);
}
