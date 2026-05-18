#include <openssl/ecdsa.h>
#include "hblk_crypto.h"

/**
 * ec_sign - Signs a set of bytes with an EC private key
 * @key: EC key containing the private key
 * @msg: Message to sign
 * @msglen: Message length
 * @sig: Buffer in which to store the signature
 *
 * Return: Pointer to the signature buffer, or NULL on failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg,
	size_t msglen, sig_t *sig)
{
	unsigned int len;

	if (!key || !msg || !sig)
		return (NULL);

	len = sig->len;
	if (ECDSA_sign(0, msg, msglen, sig->sig, &len, (EC_KEY *)key) != 1)
		return (NULL);

	sig->len = len;
	return (sig->sig);
}
