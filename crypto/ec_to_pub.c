#include <openssl/ec.h>
#include "hblk_crypto.h"

/**
 * ec_to_pub - Extracts the public key from an EC_KEY structure
 * @key: Key to extract from
 * @pub: Buffer in which to store the public key
 *
 * Return: Pointer to pub, or NULL on failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	EC_POINT const *point;
	EC_GROUP const *group;
	size_t len;

	if (!key || !pub)
		return (NULL);

	point = EC_KEY_get0_public_key(key);
	group = EC_KEY_get0_group(key);
	if (!point || !group)
		return (NULL);

	len = EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED,
		pub, EC_PUB_LEN, NULL);
	if (len != EC_PUB_LEN)
		return (NULL);

	return (pub);
}
