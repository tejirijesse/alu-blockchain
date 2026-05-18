#include "hblk_crypto.h"

/**
 * sha256 - Computes the SHA256 hash of a sequence of bytes
 * @s: Sequence of bytes to hash
 * @len: Number of bytes to hash
 * @digest: Buffer in which to store the hash
 *
 * Return: Pointer to digest, or NULL on failure
 */
uint8_t *sha256(int8_t const *s, size_t len,
	uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!s || !digest)
		return (NULL);

	SHA256((unsigned char const *)s, len, digest);
	return (digest);
}
