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
	unsigned int sig_len;

	if (!key || !msg || !sig)
		return (NULL);

	if (ECDSA_size((EC_KEY *)key) > SIG_MAX_LEN)
		return (NULL);

	sig_len = SIG_MAX_LEN;
	if (!ECDSA_sign(0, msg, msglen, sig->sig, &sig_len,
		(EC_KEY *)key))
		return (NULL);

	sig->len = sig_len;
	return (sig->sig);
}
