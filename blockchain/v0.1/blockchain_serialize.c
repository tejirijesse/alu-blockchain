#include "blockchain.h"

/**
 * write_block - Writes a block to a file
 * @node: Current list node data
 * @idx: Node index
 * @arg: File stream
 *
 * Return: 0 on success, -1 on failure
 */
static int write_block(llist_node_t node, int idx, void *arg)
{
	block_t const *block = node;
	FILE *file = arg;

	(void)idx;
	if (!block || !file || block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);
	if (fwrite(&block->info, sizeof(block->info), 1, file) != 1)
		return (-1);
	if (fwrite(&block->data.len, sizeof(block->data.len), 1, file) != 1)
		return (-1);
	if (block->data.len &&
		fwrite(block->data.buffer, block->data.len, 1, file) != 1)
		return (-1);
	if (fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, file) != 1)
		return (-1);

	return (0);
}

/**
 * blockchain_serialize - Serializes a blockchain into a file
 * @blockchain: Blockchain structure to serialize
 * @path: Path to destination file
 *
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *file;
	hblk_file_t header;
	int count;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);

	count = llist_size(blockchain->chain);
	if (count < 0)
		return (-1);

	memcpy(header.hblk_magic, HBLK_MAGIC, 4);
	memcpy(header.hblk_version, HBLK_VERSION, 3);
	header.hblk_endian = _get_endianness();
	header.hblk_blocks = count;

	file = fopen(path, "wb");
	if (!file)
		return (-1);
	if (fwrite(&header, sizeof(header), 1, file) != 1)
	{
		fclose(file);
		return (-1);
	}
	if (llist_for_each(blockchain->chain, write_block, file) != 0)
	{
		fclose(file);
		return (-1);
	}

	fclose(file);
	return (0);
}
