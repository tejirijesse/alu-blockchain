#include "cli.h"



/**
 * get_input -	Reads and process data from standard input, creates arguments
 *				from the input.
 * @av:			Pointer where to save pointer to readed input.
 * Return:		Amount of arguments.
 */
int get_input(char **av)
{
	char *str = NULL, *buffer = NULL;
	size_t size = 0, am_arg = 0;

	write(1, "cmd > ", 6);
	getline(&buffer, &size, stdin);
	str = strtok(buffer, " \n");
	while (str)
	{
		if (am_arg < 3)
			av[am_arg] = strdup(str);
		str = strtok(NULL, " \n");
		am_arg++;
	}
	free(buffer);
	if (am_arg > 3)
	{
		printf("\tWrite <help> to list available commands.\n");
		free_args(av);
	}
	return ((int)am_arg);
}


/**
 * free_args -	Frees and sets to NULL.
 * @av:			Array of commands to free.
 * Return:		void
 */
void free_args(char **av)
{
	int i = 0;

	for (; i < 3; i++)
		if (av[i])
			free(av[i]);
	memset(av, 0, 24);
}


/**
 * ch_print_mode -	Changes the printing mode.
 * @d:			Struct containing all data.
 * Return:		0 on succes, non zero otherwise.
 */
int ch_print_mode(clid_t *d)
{
	d->p_mode = (d->p_mode) ? 0 : 1;
	return (0);
}
