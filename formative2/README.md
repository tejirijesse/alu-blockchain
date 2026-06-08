# Blockchain Attendance Mining Simulation (Formative 2)

Extends the Formative 1 attendance blockchain with token transactions, a
pending pool, two ledger models (UTXO and account-based), and three mining
simulations (solo, pool, cloud). Every block is linked by `previous_hash`,
signed with an HMAC-SHA-256 commitment, and confirmed via proof of work.

## Demo Video

5–10 minute walkthrough of the system: _link to be added_.

## Dependencies

None beyond a C99 compiler and `make`. SHA-256 is implemented in
`attendance_mining.c`; there is no OpenSSL or external library to install.

## Compile

```bash
make
```

This produces the binary `./attendance_mining`.

## Run

```bash
./attendance_mining
```

Optional command-line arguments:

```bash
./attendance_mining --model utxo --difficulty 3
./attendance_mining --model account --difficulty 2
```

- `--model {utxo|account}` — choose the transaction model (default `utxo`).
- `--difficulty {1..4}` — number of leading zero hex characters required of a
  mined block hash (default `2`).

Both can also be changed from the menu while the program is running.

## Student Registry

On startup the program loads `students.txt` (one record per line, format
`student_id,full_name,course_code`). If the file is missing, a built-in
roster of `S001`–`S004` is used. Marking attendance for an ID that is not
in the loaded registry prints `ERROR: Student ID not found` and aborts.

Sample `students.txt`:

```
ALU001,John Doe,BLK101
ALU002,Jane Smith,BLK101
ALU003,Amara Diallo,BLK101
```

## Menu

| Option | Action |
|--------|--------|
| 1 | Choose transaction model (UTXO / account) |
| 2 | Set mining difficulty (1–4) |
| 3 | Mark attendance (creates a pending block + reward tx) |
| 4 | Print pending pool |
| 5 | Solo mine pending pool |
| 6 | Pool mine pending pool |
| 7 | Cloud mine pending pool |
| 8 | Print confirmed attendance |
| 9 | Print balances |
| 10 | Print UTXO set |
| 11 | Manual token transfer |
| 12 | Print account history (account model) |
| 13 | Validate chain integrity |
| 14 | Tamper detection demo |
| 15 | List registered students |
| 0 | Exit |

## Attendance Rewards

| Status | Reward | Effect |
|--------|--------|--------|
| PRESENT | 10 coins | Token transaction generated |
| LATE | 5 coins | Token transaction generated |
| ABSENT | 0 coins | No token transaction generated |

Every token transaction deducts a fixed 1-coin fee before crediting the
receiver.

## Testing Mining Simulations

Run through the flow once per mining method (script the input via `printf`
piped into the binary if you want repeatable demos):

1. Mark 2–3 students with PRESENT / LATE / ABSENT.
2. View the pending pool (option 4).
3. Run one of:
   - Option 5 — solo: shows attempt count per block, full mining reward (25).
   - Option 6 — pool: 3 simulated miners with random hash rates, 2% pool fee.
   - Option 7 — cloud: prompts for rounds (1–5) and rental fee per round.
     Try `12` for profitable, `22` for unprofitable to see the warning.
4. Option 8 — confirmed records with signature validity.
5. Option 9 — student balances; option 10 — full UTXO set.
6. Option 11 — manual transfer (UTXO or account); option 12 — account
   history (linked list) for a given student.
7. Option 13 — validate the whole chain; option 14 — demonstrate tamper
   detection by flipping a past block's status in memory.

## Edge Cases

| Case | How to trigger | Expected |
|------|----------------|----------|
| Absent student → no transaction | Mark `ABSENT` | "ABSENT recorded: no token transaction generated." |
| Unknown student ID | Mark with an ID not in `students.txt` | `ERROR: Student ID not found` |
| Insufficient balance (account) | Transfer more than the sender has | `Rejected: insufficient account balance.` |
| Reused / wrong nonce (account) | Transfer with `nonce != current+1` | `Rejected: incorrect or reused nonce.` |
| Insufficient UTXOs | Transfer more than available unspent outputs | `Rejected: insufficient UTXOs for amount plus fee.` |
| Unprofitable cloud rental | Set rental fee per round to a value > reward per round | "Warning: cloud rental is unprofitable at round N." |
| Chain tampering | Option 14 | Tamper detection prints `Block N TAMPERED` for hash + signature. |

## Clean

```bash
make clean
```
