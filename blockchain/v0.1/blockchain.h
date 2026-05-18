#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <llist.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "../../crypto/hblk_crypto.h"
#include "provided/endianness.h"

#define BLOCKCHAIN_DATA_MAX 1024
#define HBLK_MAGIC "HBLK"
#define HBLK_VERSION "0.1"

/**
 * struct blockchain_s - Blockchain structure
 * @chain: Linked list of pointers to block_t
 */
typedef struct blockchain_s
{
	llist_t *chain;
} blockchain_t;

/**
 * struct block_info_s - Block info structure
 * @index: Index of the Block in the Blockchain
 * @difficulty: Difficulty of proof of work
 * @timestamp: Time the Block was created at
 * @nonce: Salt value used to alter the Block hash
 * @prev_hash: Hash of the previous Block
 */
typedef struct block_info_s
{
	uint32_t index;
	uint32_t difficulty;
	uint64_t timestamp;
	uint64_t nonce;
	uint8_t prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;

/**
 * struct block_data_s - Block data
 * @buffer: Data buffer
 * @len: Data size in bytes
 */
typedef struct block_data_s
{
	int8_t buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t len;
} block_data_t;

/**
 * struct block_s - Block structure
 * @info: Block info
 * @data: Block data
 * @hash: 256-bit digest of the Block
 */
typedef struct block_s
{
	block_info_t info;
	block_data_t data;
	uint8_t hash[SHA256_DIGEST_LENGTH];
} block_t;

/**
 * struct hblk_file_s - Block file format
 * @hblk_magic: Magic bytes
 * @hblk_version: Blockchain version
 * @hblk_endian: File endianness
 * @hblk_blocks: Number of blocks in the file
 */
typedef struct hblk_file_s
{
	int8_t hblk_magic[4];
	int8_t hblk_version[3];
	int8_t hblk_endian;
	uint32_t hblk_blocks;
} hblk_file_t;

blockchain_t *blockchain_create(void);
block_t *block_create(block_t const *prev, int8_t const *data,
	uint32_t data_len);
void block_destroy(block_t *block);
void blockchain_destroy(blockchain_t *blockchain);
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(char const *path);
int block_is_valid(block_t const *block, block_t const *prev_block);

void _blockchain_destroy(blockchain_t *blockchain);
void _blockchain_print(blockchain_t const *blockchain);
void _blockchain_print_brief(blockchain_t const *blockchain);

#endif /* BLOCKCHAIN_H */
