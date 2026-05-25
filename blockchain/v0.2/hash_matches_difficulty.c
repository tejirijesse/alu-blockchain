#include "blockchain.h"

/**
 * hash_matches_difficulty - Checks if hash matches difficulty
 * @hash: Hash to check
 * @difficulty: Difficulty to match
 *
 * Return: 1 if hash matches difficulty, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
	uint32_t difficulty)
{
	uint32_t i, bytes, bits;

	if (!hash || difficulty > SHA256_DIGEST_LENGTH * 8)
		return (0);

	bytes = difficulty / 8;
	bits = difficulty % 8;

	for (i = 0; i < bytes; i++)
	{
		if (hash[i] != 0)
			return (0);
	}
	if (bits && (hash[bytes] >> (8 - bits)))
		return (0);

	return (1);
}
