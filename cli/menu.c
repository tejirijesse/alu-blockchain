#include "cli.h"

/* Array with the functions names. */
char const *alpha_opt[] = {
	"wallet_load",
	"wallet_create",
	"wallet_save",

	"send",

	"mine",

	"info",
	"change_print_mode",
	"coin_dist",
	"bc_dist",
	"print_block",
	"print_local_pool",
	"print_unspent",

	"bc_init",
	"load",
	"save",

	"free_data",
	"help",
	"exit",
	NULL
};


/* Array with functions pointers. */
int (*alpha_func[])(clid_t *) = {
	load_wallet,
	create_wallet,
	save_wallet,

	send,

	mine,

	info,
	ch_print_mode,
	coin_dist,
	bc_dist,
	print_block,
	print_lp,
	print_unspent,

	bc_init,
	load_blockchain,
	save_blockchain,

	free_data,
	basic_help,
	exit_cli,
	NULL
};


/**
 * alpha_ret -	Retrieves the corresponding function from a function
 *				pointer array.
 *
 * @str:	String containing command name.
 *
 * Return:	Function pointer to the requested command if str matches,
 *			NULL otherwise.
 */
int (*alpha_ret(char *str))(clid_t *d)
{
	int i = 0;

	while (alpha_opt[i] && memcmp(alpha_opt[i], str, strlen(alpha_opt[i])))
		i++;
	return (alpha_func[i]);
}
