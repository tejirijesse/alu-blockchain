#include "cli.h"



/**
 * exit_cli -	Exits the main cli.
 * @d:			Struct containing all variables.
 * Return:		0 on success, non zero otherwise.
 */
int exit_cli(clid_t *d)
{
	d->exit_cli = 1;
	return (0);
}
