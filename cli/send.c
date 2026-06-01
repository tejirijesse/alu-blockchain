#include "cli.h"



/**
 * send -	Creates a transaction.
 * @d:		Struct containing all data.
 * Return:	0 on succes, non zero otherwise.
 */
int send(clid_t *d)
{
	transaction_t *tx = NULL;
	EC_KEY *receiver = NULL;
	unsigned long amount;
	uint8_t *r;

	if (!d)
		return (s_error(40));
	if (!d->av[1] || !d->av[2])
		return (s_error(41));
	if (!d->key)
		return (s_error(42));
	if (!d->bc)
		return (s_error(43));
	if (!d->bc->unspent)
		return (s_error(44));
	if (!d->local_pool)
		return (s_error(50));
	if (check_num(d->av[1]))
		return (s_error(48));
	r = unhex(d->av[2]);
	if (!r)
		return (s_error(49));
	receiver = ec_from_pub(r);
	free(r);
	if (!receiver)
		return (s_error(45));
	amount = strtoul(d->av[1], NULL, 10);
	if (!amount || amount > UINT32_MAX)
	{
		EC_KEY_free(receiver);
		return (s_error(48));
	}
	tx = transaction_create(d->key, receiver, (uint32_t)amount,
		d->bc->unspent);
	EC_KEY_free(receiver);
	if (!tx)
		return (s_error(46));
	if (!transaction_is_valid(tx, d->bc->unspent))
	{
		transaction_destroy(tx);
		return (s_error(47));
	}
	if (llist_add_node(d->local_pool, (llist_node_t)tx, ADD_NODE_REAR))
	{
		transaction_destroy(tx);
		return (s_error(51));
	}
	printf("Transaction created.\n");
	return (0);
}


/**
 * check_num -	checks if a string is a valid unsigned intger
 * @str:		string to converto to integer
 * Return:		0 if valid, non zero otherwise
 */
int check_num(char *str)
{
	int i = 0;

	if (!str || !str[0])
		return (-1);
	while (str[i])
	{
		if ((str[i] > '9') || (str[i] < '0'))
			return (-1);
		i++;
	}
	if (i > 10)
		return (-1);
	return (0);
}


/**
 * hex_value -	Converts a hexadecimal digit to its integer value.
 * @c:		Hexadecimal digit.
 * Return:	Integer value, or -1 if c is not hexadecimal.
 */
static int hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (-1);
}


/**
 * unhex -	Converts a hexadecimal string to a byte buffer.
 * @s:		Hex string to convert.
 * Return:	Byte buffer, or NULL on failure.
 */
uint8_t *unhex(char *s)
{
	int high, low;
	size_t i;
	uint8_t *uh;

	if (!s || strlen(s) != (2 * EC_PUB_LEN))
		return (NULL);
	uh = malloc(EC_PUB_LEN);
	if (!uh)
		return (NULL);
	for (i = 0; i < EC_PUB_LEN; i++)
	{
		high = hex_value(s[i * 2]);
		low = hex_value(s[i * 2 + 1]);
		if (high < 0 || low < 0)
		{
			free(uh);
			return (NULL);
		}
		uh[i] = (uint8_t)((high << 4) | low);
	}
	return (uh);
}


/**
 * s_error -	Handles errors.
 * @no:			Error index.
 * Return:		Number that represents the error.
 */
int s_error(int no)
{
	if (no == 40)
		printf("No struct with data specified.\n");
	else if (no == 41)
		printf("No ammount or address specified.\n");
	else if (no == 42)
		printf("No sender specified.\n");
	else if (no == 43)
		printf("No blockchain initialized.\n");
	else if (no == 44)
		printf("No unspent transaction available.\n");
	else if (no == 45)
		printf("Wrong receiver address.\n");
	else if (no == 46)
		printf("Couldn't create transaction.\n");
	else if (no == 47)
		printf("Transaction wasn't valid.\n");
	else if (no == 48)
		printf("Amount not valid.\n");
	else if (no == 49)
		printf("String to key not valid.\n");
	else if (no == 50)
		printf("No local transaction pool available.\n");
	else if (no == 51)
		printf("Couldn't add transaction to local pool.\n");
	return (no);
}
