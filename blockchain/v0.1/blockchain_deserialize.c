#include "blockchain.h"

/**
 * read_block - Reads one block from a file
 * @file: Source file
 * @swap: Whether integer values must be endian-swapped
 *
 * Return: Pointer to the block, or NULL on failure
 */
static block_t *read_block(FILE *file, int swap)
{
	block_t *block;

	block = calloc(1, sizeof(*block));
	if (!block)
		return (NULL);

	if (fread(&block->info, sizeof(block->info), 1, file) != 1)
		return (free(block), NULL);
	if (swap)
	{
		SWAPENDIAN(block->info.index);
		SWAPENDIAN(block->info.difficulty);
		SWAPENDIAN(block->info.timestamp);
		SWAPENDIAN(block->info.nonce);
	}
	if (fread(&block->data.len, sizeof(block->data.len), 1, file) != 1)
		return (free(block), NULL);
	if (swap)
		SWAPENDIAN(block->data.len);
	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (free(block), NULL);
	if (block->data.len &&
		fread(block->data.buffer, block->data.len, 1, file) != 1)
		return (free(block), NULL);
	if (fread(block->hash, SHA256_DIGEST_LENGTH, 1, file) != 1)
		return (free(block), NULL);

	return (block);
}

/**
 * read_blocks - Reads blocks from a file
 * @file: Source file
 * @count: Number of blocks to read
 * @swap: Whether integer values must be endian-swapped
 *
 * Return: Pointer to a list of blocks, or NULL on failure
 */
static llist_t *read_blocks(FILE *file, uint32_t count, int swap)
{
	llist_t *list;
	block_t *block;
	uint32_t i;

	list = llist_create(MT_SUPPORT_TRUE);
	if (!list)
		return (NULL);

	for (i = 0; i < count; i++)
	{
		block = read_block(file, swap);
		if (!block || llist_add_node(list, block, ADD_NODE_REAR) != 0)
		{
			free(block);
			llist_destroy(list, 1, NULL);
			return (NULL);
		}
	}

	return (list);
}

/**
 * blockchain_deserialize - Deserializes a blockchain from a file
 * @path: Path to serialized blockchain file
 *
 * Return: Pointer to deserialized blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file;
	hblk_file_t header;
	blockchain_t *blockchain;
	int swap;

	if (!path)
		return (NULL);

	file = fopen(path, "rb");
	if (!file)
		return (NULL);
	if (fread(&header, sizeof(header), 1, file) != 1)
		return (fclose(file), NULL);
	if (memcmp(header.hblk_magic, HBLK_MAGIC, 4) != 0 ||
		memcmp(header.hblk_version, HBLK_VERSION, 3) != 0)
		return (fclose(file), NULL);
	if (header.hblk_endian != 1 && header.hblk_endian != 2)
		return (fclose(file), NULL);

	swap = header.hblk_endian != _get_endianness();
	if (swap)
		SWAPENDIAN(header.hblk_blocks);

	blockchain = calloc(1, sizeof(*blockchain));
	if (!blockchain)
		return (fclose(file), NULL);
	blockchain->chain = read_blocks(file, header.hblk_blocks, swap);
	fclose(file);
	if (!blockchain->chain)
	{
		free(blockchain);
		return (NULL);
	}

	return (blockchain);
}
