#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 4
#define MAX_NAME 32
#define MAX_COURSE 32
#define HASH_HEX 65
#define TX_FEE 1
#define PRESENT_REWARD 10
#define LATE_REWARD 5
#define BLOCK_REWARD 25
#define CLOUD_REWARD 18
#define CLOUD_FEE 12

typedef enum model_e
{
	MODEL_UTXO,
	MODEL_ACCOUNT
} model_t;

typedef struct history_s
{
	char sender[16];
	char receiver[16];
	int amount;
	int fee;
	unsigned int nonce;
	struct history_s *next;
} history_t;

typedef struct student_s
{
	char id[16];
	char name[MAX_NAME];
	int balance;
	unsigned int nonce;
	history_t *history;
} student_t;

typedef struct utxo_s
{
	char id[HASH_HEX];
	char owner[16];
	int amount;
	int spent;
	struct utxo_s *next;
} utxo_t;

typedef struct attendance_s
{
	char student_id[16];
	char student_name[MAX_NAME];
	char course[MAX_COURSE];
	char status[12];
	char timestamp[32];
	int token_reward;
	char tx_id[HASH_HEX];
	unsigned int nonce;
	char hash[HASH_HEX];
	struct attendance_s *next;
} attendance_t;

typedef struct app_s
{
	student_t students[MAX_STUDENTS];
	utxo_t *utxos;
	attendance_t *pending;
	attendance_t *confirmed;
	model_t model;
	int difficulty;
	int next_utxo;
	int next_block;
} app_t;

typedef struct sha256_ctx_s
{
	uint8_t data[64];
	uint32_t datalen;
	unsigned long long bitlen;
	uint32_t state[8];
} sha256_ctx_t;

static const uint32_t k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t rotr(uint32_t value, uint32_t bits)
{
	return ((value >> bits) | (value << (32 - bits)));
}

static void sha256_transform(sha256_ctx_t *ctx, const uint8_t data[])
{
	uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	for (i = 0, j = 0; i < 16; i++, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) |
			(data[j + 2] << 8) | data[j + 3];
	for (; i < 64; i++)
	{
		t1 = rotr(m[i - 2], 17) ^ rotr(m[i - 2], 19) ^ (m[i - 2] >> 10);
		t2 = rotr(m[i - 15], 7) ^ rotr(m[i - 15], 18) ^ (m[i - 15] >> 3);
		m[i] = t1 + m[i - 7] + t2 + m[i - 16];
	}
	a = ctx->state[0], b = ctx->state[1], c = ctx->state[2];
	d = ctx->state[3], e = ctx->state[4], f = ctx->state[5];
	g = ctx->state[6], h = ctx->state[7];
	for (i = 0; i < 64; i++)
	{
		t1 = h + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) +
			((e & f) ^ (~e & g)) + k[i] + m[i];
		t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) +
			((a & b) ^ (a & c) ^ (b & c));
		h = g, g = f, f = e, e = d + t1;
		d = c, c = b, b = a, a = t1 + t2;
	}
	ctx->state[0] += a, ctx->state[1] += b, ctx->state[2] += c;
	ctx->state[3] += d, ctx->state[4] += e, ctx->state[5] += f;
	ctx->state[6] += g, ctx->state[7] += h;
}

static void sha256_init(sha256_ctx_t *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(sha256_ctx_t *ctx, const uint8_t data[], size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
	{
		ctx->data[ctx->datalen++] = data[i];
		if (ctx->datalen == 64)
		{
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

static void sha256_final(sha256_ctx_t *ctx, uint8_t hash[])
{
	uint32_t i;

	i = ctx->datalen;
	ctx->data[i++] = 0x80;
	if (ctx->datalen < 56)
	{
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else
	{
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}
	ctx->bitlen += ctx->datalen * 8;
	for (i = 0; i < 8; i++)
		ctx->data[63 - i] = (uint8_t)(ctx->bitlen >> (i * 8));
	sha256_transform(ctx, ctx->data);
	for (i = 0; i < 4; i++)
	{
		hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0xff;
		hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0xff;
		hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0xff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0xff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0xff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0xff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0xff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0xff;
	}
}

static void sha256_hex(const char *input, char out[HASH_HEX])
{
	sha256_ctx_t ctx;
	uint8_t hash[32];
	int i;

	sha256_init(&ctx);
	sha256_update(&ctx, (const uint8_t *)input, strlen(input));
	sha256_final(&ctx, hash);
	for (i = 0; i < 32; i++)
		sprintf(out + i * 2, "%02x", hash[i]);
	out[64] = '\0';
}

static student_t *find_student(app_t *app, const char *id)
{
	int i;

	for (i = 0; i < MAX_STUDENTS; i++)
		if (strcmp(app->students[i].id, id) == 0)
			return (&app->students[i]);
	return (NULL);
}

static void add_history(student_t *student, const char *sender,
	const char *receiver, int amount, int fee)
{
	history_t *entry;

	entry = malloc(sizeof(*entry));
	if (!entry)
		return;
	strcpy(entry->sender, sender);
	strcpy(entry->receiver, receiver);
	entry->amount = amount;
	entry->fee = fee;
	entry->nonce = student->nonce;
	entry->next = student->history;
	student->history = entry;
}

static void add_utxo(app_t *app, const char *owner, int amount)
{
	utxo_t *node;
	char data[128];

	if (amount <= 0)
		return;
	node = malloc(sizeof(*node));
	if (!node)
		return;
	snprintf(data, sizeof(data), "%s:%d:%d", owner, amount, app->next_utxo++);
	sha256_hex(data, node->id);
	strcpy(node->owner, owner);
	node->amount = amount;
	node->spent = 0;
	node->next = app->utxos;
	app->utxos = node;
}

static int utxo_balance(app_t *app, const char *owner)
{
	utxo_t *node;
	int total;

	total = 0;
	for (node = app->utxos; node; node = node->next)
		if (!node->spent && strcmp(node->owner, owner) == 0)
			total += node->amount;
	return (total);
}

static void print_utxos(app_t *app)
{
	utxo_t *node;

	printf("\nUTXO set:\n");
	for (node = app->utxos; node; node = node->next)
		printf("  %.12s owner=%s amount=%d spent=%s\n",
			node->id, node->owner, node->amount,
			node->spent ? "yes" : "no");
}

static void apply_reward(app_t *app, attendance_t *record)
{
	student_t *student;
	int credited;

	if (record->token_reward <= 0)
		return;
	student = find_student(app, record->student_id);
	if (!student)
		return;
	credited = record->token_reward - TX_FEE;
	if (credited < 0)
		credited = 0;
	if (app->model == MODEL_ACCOUNT)
	{
		student->balance += credited;
		add_history(student, "SYSTEM", student->id, credited, TX_FEE);
	}
	else
		add_utxo(app, student->id, credited);
}

static void print_balances(app_t *app)
{
	int i, balance;

	printf("\nStudent balances using %s model:\n",
		app->model == MODEL_UTXO ? "UTXO" : "account");
	for (i = 0; i < MAX_STUDENTS; i++)
	{
		balance = app->model == MODEL_UTXO ?
			utxo_balance(app, app->students[i].id) : app->students[i].balance;
		printf("  %s %-10s balance=%d nonce=%u\n", app->students[i].id,
			app->students[i].name, balance, app->students[i].nonce);
	}
}

static int reward_for_status(const char *status)
{
	if (strcmp(status, "PRESENT") == 0)
		return (PRESENT_REWARD);
	if (strcmp(status, "LATE") == 0)
		return (LATE_REWARD);
	return (0);
}

static void make_timestamp(char out[32])
{
	time_t now;
	struct tm *tm_now;

	now = time(NULL);
	tm_now = localtime(&now);
	strftime(out, 32, "%Y-%m-%d %H:%M:%S", tm_now);
}

static void mark_attendance(app_t *app)
{
	attendance_t *record;
	student_t *student;
	char id[16], course[MAX_COURSE], status[12], data[256];

	printf("Student ID (S001-S004): ");
	scanf("%15s", id);
	student = find_student(app, id);
	if (!student)
	{
		printf("Unknown student.\n");
		return;
	}
	printf("Course: ");
	scanf("%31s", course);
	printf("Status (PRESENT/LATE/ABSENT): ");
	scanf("%11s", status);
	if (strcmp(status, "PRESENT") && strcmp(status, "LATE") &&
		strcmp(status, "ABSENT"))
	{
		printf("Invalid status.\n");
		return;
	}
	record = calloc(1, sizeof(*record));
	if (!record)
		return;
	strcpy(record->student_id, student->id);
	strcpy(record->student_name, student->name);
	strcpy(record->course, course);
	strcpy(record->status, status);
	make_timestamp(record->timestamp);
	record->token_reward = reward_for_status(status);
	snprintf(data, sizeof(data), "%s:%s:%s:%s:%d", record->student_id,
		record->course, record->status, record->timestamp,
		record->token_reward);
	sha256_hex(data, record->tx_id);
	record->next = app->pending;
	app->pending = record;
	printf("Attendance placed in pending pool. Transaction reward=%d tx=%.12s\n",
		record->token_reward, record->tx_id);
}

static void print_pending(app_t *app)
{
	attendance_t *node;

	printf("\nPending pool:\n");
	for (node = app->pending; node; node = node->next)
		printf("  %s %s %s reward=%d tx=%.12s\n",
			node->student_id, node->course, node->status,
			node->token_reward, node->tx_id);
}

static int hash_meets_difficulty(const char *hash, int difficulty)
{
	int i;

	for (i = 0; i < difficulty; i++)
		if (hash[i] != '0')
			return (0);
	return (1);
}

static unsigned long mine_record(attendance_t *record, int difficulty)
{
	char data[512];
	unsigned long attempts;

	attempts = 0;
	record->nonce = 0;
	do {
		record->nonce++;
		attempts++;
		snprintf(data, sizeof(data), "%s:%s:%s:%s:%d:%s:%u",
			record->student_id, record->course, record->status,
			record->timestamp, record->token_reward, record->tx_id,
			record->nonce);
		sha256_hex(data, record->hash);
	} while (!hash_meets_difficulty(record->hash, difficulty));
	return (attempts);
}

static void confirm_all(app_t *app, const char *method)
{
	attendance_t *node, *next;

	node = app->pending;
	while (node)
	{
		next = node->next;
		apply_reward(app, node);
		node->next = app->confirmed;
		app->confirmed = node;
		printf("Confirmed by %s: %s %s %s reward=%d valid_signature=yes\n",
			method, node->student_name, node->course, node->status,
			node->token_reward);
		node = next;
	}
	app->pending = NULL;
}

static void solo_mine(app_t *app)
{
	attendance_t *node;
	unsigned long attempts, total;

	if (!app->pending)
	{
		printf("No pending attendance blocks.\n");
		return;
	}
	total = 0;
	for (node = app->pending; node; node = node->next)
	{
		attempts = mine_record(node, app->difficulty);
		total += attempts;
		printf("Solo mined %.12s attempts=%lu hash=%s\n",
			node->tx_id, attempts, node->hash);
	}
	printf("Solo miner reward=%d total_attempts=%lu\n", BLOCK_REWARD, total);
	confirm_all(app, "solo mining");
}

static void pool_mine(app_t *app)
{
	int i, miners, rates[5], attempts[5], total, reward_pool;
	double percent, reward;
	attendance_t *node;

	if (!app->pending)
	{
		printf("No pending attendance blocks.\n");
		return;
	}
	miners = 3;
	total = 0;
	srand((unsigned int)time(NULL));
	for (i = 0; i < miners; i++)
	{
		rates[i] = 50 + rand() % 151;
		attempts[i] = rates[i] * (1 + rand() % 5);
		total += attempts[i];
	}
	for (node = app->pending; node; node = node->next)
		mine_record(node, app->difficulty);
	reward_pool = BLOCK_REWARD - (BLOCK_REWARD * 2 / 100);
	printf("\nPool mining rewards after 2%% pool fee:\n");
	printf("Miner | Attempts | Share %% | Reward\n");
	for (i = 0; i < miners; i++)
	{
		percent = total ? (attempts[i] * 100.0 / total) : 0.0;
		reward = reward_pool * percent / 100.0;
		printf("M%-5d| %-9d| %6.2f | %6.2f\n",
			i + 1, attempts[i], percent, reward);
	}
	confirm_all(app, "pool mining");
}

static void cloud_mine(app_t *app)
{
	int rounds, i, fees, gross;

	if (!app->pending)
	{
		printf("No pending attendance blocks.\n");
		return;
	}
	printf("Rental duration rounds (1-5): ");
	scanf("%d", &rounds);
	if (rounds < 1)
		rounds = 1;
	if (rounds > 5)
		rounds = 5;
	fees = 0;
	gross = 0;
	for (i = 1; i <= rounds; i++)
	{
		fees += CLOUD_FEE;
		gross += CLOUD_REWARD;
		printf("Round %d gross=%d fees=%d net=%d\n",
			i, gross, fees, gross - fees);
		if (fees > gross)
			printf("Warning: cloud rental is unprofitable at round %d.\n", i);
	}
	printf("Cloud mining summary gross=%d total_fees=%d net_profit=%d\n",
		gross, fees, gross - fees);
	solo_mine(app);
}

static void transfer_utxo(app_t *app, const char *sender, const char *receiver,
	int amount)
{
	utxo_t *node;
	int needed, total;

	needed = amount + TX_FEE;
	total = 0;
	for (node = app->utxos; node && total < needed; node = node->next)
	{
		if (!node->spent && strcmp(node->owner, sender) == 0)
		{
			node->spent = 1;
			total += node->amount;
		}
	}
	if (total < needed)
	{
		printf("Rejected: insufficient UTXOs for amount plus fee.\n");
		return;
	}
	add_utxo(app, receiver, amount);
	if (total > needed)
		add_utxo(app, sender, total - needed);
	printf("UTXO transfer accepted with fee=%d change=%d\n",
		TX_FEE, total - needed);
}

static void transfer_account(app_t *app, const char *sender_id,
	const char *receiver_id, int amount, unsigned int nonce)
{
	student_t *sender, *receiver;

	sender = find_student(app, sender_id);
	receiver = find_student(app, receiver_id);
	if (!sender || !receiver)
	{
		printf("Rejected: unknown student.\n");
		return;
	}
	if (nonce != sender->nonce + 1)
	{
		printf("Rejected: incorrect or reused nonce.\n");
		return;
	}
	if (sender->balance < amount + TX_FEE)
	{
		printf("Rejected: insufficient account balance.\n");
		return;
	}
	sender->balance -= amount + TX_FEE;
	receiver->balance += amount;
	sender->nonce = nonce;
	add_history(sender, sender_id, receiver_id, amount, TX_FEE);
	printf("Account transfer accepted. Sender nonce is now %u.\n", nonce);
}

static void manual_transfer(app_t *app)
{
	char sender[16], receiver[16];
	int amount;
	unsigned int nonce;

	printf("Sender ID: ");
	scanf("%15s", sender);
	printf("Receiver ID: ");
	scanf("%15s", receiver);
	printf("Amount: ");
	scanf("%d", &amount);
	if (app->model == MODEL_UTXO)
		transfer_utxo(app, sender, receiver, amount);
	else
	{
		printf("Nonce: ");
		scanf("%u", &nonce);
		transfer_account(app, sender, receiver, amount, nonce);
	}
}

static void print_history(app_t *app)
{
	char id[16];
	student_t *student;
	history_t *entry;

	printf("Student ID: ");
	scanf("%15s", id);
	student = find_student(app, id);
	if (!student)
	{
		printf("Unknown student.\n");
		return;
	}
	printf("History for %s:\n", student->id);
	for (entry = student->history; entry; entry = entry->next)
		printf("  %s -> %s amount=%d fee=%d nonce=%u\n",
			entry->sender, entry->receiver, entry->amount,
			entry->fee, entry->nonce);
}

static void print_confirmed(app_t *app)
{
	attendance_t *node;

	printf("\nConfirmed attendance records:\n");
	for (node = app->confirmed; node; node = node->next)
		printf("  %s %-10s course=%s status=%s time=%s reward=%d "
			"tx=%.12s signature_valid=yes\n",
			node->student_id, node->student_name, node->course,
			node->status, node->timestamp, node->token_reward, node->tx_id);
}

static void init_app(app_t *app)
{
	memset(app, 0, sizeof(*app));
	strcpy(app->students[0].id, "S001");
	strcpy(app->students[0].name, "Alice");
	strcpy(app->students[1].id, "S002");
	strcpy(app->students[1].name, "Bob");
	strcpy(app->students[2].id, "S003");
	strcpy(app->students[2].name, "Chantal");
	strcpy(app->students[3].id, "S004");
	strcpy(app->students[3].name, "David");
	app->model = MODEL_UTXO;
	app->difficulty = 2;
}

static void choose_model(app_t *app)
{
	int choice;

	printf("1. UTXO model\n2. Account model\nChoose: ");
	scanf("%d", &choice);
	app->model = choice == 2 ? MODEL_ACCOUNT : MODEL_UTXO;
	printf("Using %s model.\n", app->model == MODEL_UTXO ? "UTXO" : "account");
}

static void set_difficulty(app_t *app, int value)
{
	if (value < 1)
		value = 1;
	if (value > 4)
		value = 4;
	app->difficulty = value;
}

static void parse_args(app_t *app, int argc, char **argv)
{
	int i;

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--model") == 0 && i + 1 < argc)
		{
			i++;
			app->model = strcmp(argv[i], "account") == 0 ?
				MODEL_ACCOUNT : MODEL_UTXO;
		}
		else if (strcmp(argv[i], "--difficulty") == 0 && i + 1 < argc)
			set_difficulty(app, atoi(argv[++i]));
	}
}

static void menu(void)
{
	printf("\n1 Choose transaction model\n");
	printf("2 Set mining difficulty\n");
	printf("3 Mark attendance\n");
	printf("4 Print pending pool\n");
	printf("5 Solo mine pending pool\n");
	printf("6 Pool mine pending pool\n");
	printf("7 Cloud mine pending pool\n");
	printf("8 Print confirmed attendance\n");
	printf("9 Print balances\n");
	printf("10 Print UTXO set\n");
	printf("11 Manual token transfer\n");
	printf("12 Print account history\n");
	printf("0 Exit\n> ");
}

int main(int argc, char **argv)
{
	app_t app;
	int choice, difficulty;

	init_app(&app);
	parse_args(&app, argc, argv);
	while (1)
	{
		menu();
		if (scanf("%d", &choice) != 1)
			break;
		if (choice == 0)
			break;
		if (choice == 1)
			choose_model(&app);
		else if (choice == 2)
		{
			printf("Difficulty (1-4): ");
			scanf("%d", &difficulty);
			set_difficulty(&app, difficulty);
		}
		else if (choice == 3)
			mark_attendance(&app);
		else if (choice == 4)
			print_pending(&app);
		else if (choice == 5)
			solo_mine(&app);
		else if (choice == 6)
			pool_mine(&app);
		else if (choice == 7)
			cloud_mine(&app);
		else if (choice == 8)
			print_confirmed(&app);
		else if (choice == 9)
			print_balances(&app);
		else if (choice == 10)
			print_utxos(&app);
		else if (choice == 11)
			manual_transfer(&app);
		else if (choice == 12)
			print_history(&app);
	}
	return (0);
}
