#include <openssl/ecdsa.h>
#include "hblk_crypto.h"

/**
 * ec_verify - Verifies a signature with an EC public key
 * @key: EC key containing the public key
 * @msg: Signed message
 * @msglen: Message length
 * @sig: Signature to verify
 *
 * Return: 1 if the signature is valid, 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg,
	size_t msglen, sig_t const *sig)
{
	int ret, size;

	if (!key || !msg || !sig)
		return (0);
	if (sig->len == 0 || sig->len > SIG_MAX_LEN)
		return (0);
	size = ECDSA_size((EC_KEY *)key);
	if (size <= 0 || sig->len > (size_t)size)
		return (0);

	ret = ECDSA_verify(0, msg, msglen, sig->sig, sig->len,
		(EC_KEY *)key);

	return (ret == 1);
}
