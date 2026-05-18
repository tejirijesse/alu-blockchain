#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/pem.h>
#include "hblk_crypto.h"

/**
 * write_private_key - Writes an EC private key to disk
 * @key: Key to write
 * @path: Destination path
 *
 * Return: 1 on success, 0 on failure
 */
static int write_private_key(EC_KEY *key, char const *path)
{
	FILE *fp;
	int ret;

	fp = fopen(path, "w");
	if (!fp)
		return (0);
	ret = PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL);
	fclose(fp);
	return (ret == 1);
}

/**
 * write_public_key - Writes an EC public key to disk
 * @key: Key to write
 * @path: Destination path
 *
 * Return: 1 on success, 0 on failure
 */
static int write_public_key(EC_KEY *key, char const *path)
{
	FILE *fp;
	int ret;

	fp = fopen(path, "w");
	if (!fp)
		return (0);
	ret = PEM_write_EC_PUBKEY(fp, key);
	fclose(fp);
	return (ret == 1);
}

/**
 * ec_save - Saves an EC key pair to disk
 * @key: Key pair to save
 * @folder: Folder in which to save the key files
 *
 * Return: 1 on success, 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char *priv_path, *pub_path;
	size_t len;
	int ret;

	if (!key || !folder)
		return (0);

	if (mkdir(folder, 0755) == -1)
	{
		struct stat st;

		if (stat(folder, &st) == -1 || !S_ISDIR(st.st_mode))
			return (0);
	}

	len = strlen(folder);
	priv_path = malloc(len + strlen("/key.pem") + 1);
	pub_path = malloc(len + strlen("/key_pub.pem") + 1);
	if (!priv_path || !pub_path)
	{
		free(priv_path);
		free(pub_path);
		return (0);
	}

	sprintf(priv_path, "%s/key.pem", folder);
	sprintf(pub_path, "%s/key_pub.pem", folder);
	ret = write_private_key(key, priv_path) && write_public_key(key, pub_path);

	free(priv_path);
	free(pub_path);
	return (ret);
}
