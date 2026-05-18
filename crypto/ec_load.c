#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

/**
 * read_public_key - Reads an EC public key from disk
 * @path: Source path
 *
 * Return: Pointer to the public key, or NULL on failure
 */
static EC_KEY *read_public_key(char const *path)
{
	FILE *fp;
	EC_KEY *key;

	fp = fopen(path, "r");
	if (!fp)
		return (NULL);
	key = PEM_read_EC_PUBKEY(fp, NULL, NULL, NULL);
	fclose(fp);
	return (key);
}

/**
 * ec_load - Loads an EC key pair from disk
 * @folder: Folder from which to load the key files
 *
 * Return: Pointer to the loaded key pair, or NULL on failure
 */
EC_KEY *ec_load(char const *folder)
{
	char *priv_path, *pub_path;
	size_t len;
	FILE *fp;
	EC_KEY *key, *pub_key;
	EC_POINT const *pub_point;

	if (!folder)
		return (NULL);

	len = strlen(folder);
	priv_path = malloc(len + strlen("/") + strlen(PRI_FILENAME) + 1);
	pub_path = malloc(len + strlen("/") + strlen(PUB_FILENAME) + 1);
	if (!priv_path || !pub_path)
	{
		free(priv_path);
		free(pub_path);
		return (NULL);
	}
	sprintf(priv_path, "%s/%s", folder, PRI_FILENAME);
	sprintf(pub_path, "%s/%s", folder, PUB_FILENAME);

	fp = fopen(priv_path, "r");
	if (!fp)
	{
		free(priv_path);
		free(pub_path);
		return (NULL);
	}
	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);

	pub_key = read_public_key(pub_path);
	free(priv_path);
	free(pub_path);
	if (!key || !pub_key)
	{
		EC_KEY_free(key);
		EC_KEY_free(pub_key);
		return (NULL);
	}

	pub_point = EC_KEY_get0_public_key(pub_key);
	if (!pub_point || !EC_KEY_set_public_key(key, pub_point) ||
		!EC_KEY_check_key(key))
	{
		EC_KEY_free(key);
		EC_KEY_free(pub_key);
		return (NULL);
	}

	EC_KEY_free(pub_key);
	return (key);
}
