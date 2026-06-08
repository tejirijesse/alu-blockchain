# Technical Report: Blockchain Transaction Models and Mining Simulation

## 1. Attendance to Transactions

When a student is marked, the system immediately constructs an attendance
block holding the student's ID, name, course code (looked up from the
registry), status, timestamp, the token reward for that status, and a
`tx_id` equal to the SHA-256 hash of the reward transaction data. The
reward is determined by the status:

| Status | Token reward | Transaction generated |
|--------|--------------|------------------------|
| PRESENT | 10 coins | yes |
| LATE | 5 coins | yes |
| ABSENT | 0 coins | no |

`ABSENT` records are still placed in the pending pool so the attendance is
captured, but they carry a `token_reward` of `0` and no balance update
happens when they are confirmed.

## 2. Pending Pool and Mining

Attendance records are NOT immediately permanent. They sit in a singly
linked pending pool. Mining is the confirmation step that promotes a block
from pending to confirmed:

1. The miner assigns the next chain `index` and copies the current chain
   tip into the block's `previous_hash`.
2. The miner signs the block (HMAC-style SHA-256 over the signed fields
   plus a system secret).
3. Proof of work increments a `nonce` and recomputes
   `SHA-256(index || student_id || course || status || timestamp || reward
   || tx_id || previous_hash || signature || nonce)` until the hash starts
   with the required number of leading `0` characters (the difficulty).
4. The block's hash becomes the new chain tip; the block is appended to
   the confirmed list and the token transaction updates balances under the
   active model.

Because the PoW input includes both `previous_hash` and `signature`, any
change to a prior block invalidates that block's hash (option 13 catches
this) and forces re-mining of every block that follows.

## 3. UTXO Implementation

Each token credit (reward or transfer) appends a new node to a UTXO linked
list. A UTXO holds an `id` (SHA-256 of `owner:amount:counter`), the
`owner`, the `amount`, and a `spent` flag.

- **Balance** for a student is `Σ amount` over all UTXOs owned by them
  where `spent == 0`.
- **Reward credit:** confirming an attendance block with reward `R > 0`
  creates a UTXO of value `R - 1` for the student (the fixed 1-coin fee
  is destroyed). The full UTXO set is printed after every confirmed block
  so the grader can watch the set grow.
- **Spending:** a transfer walks the sender's unspent UTXOs marking them
  spent until total ≥ amount + fee. If total < amount + fee, the transfer
  is rejected. If total > amount + fee, the excess is returned to the
  sender as a fresh change UTXO. The receiver gets a new UTXO worth
  exactly `amount`.
- **Double-spend prevention** is built in: once a UTXO is marked
  `spent = 1`, every subsequent transfer skips it.

## 4. Account-Based Implementation

Each student record holds a `balance`, a `nonce` (initially 0), and a
linked-list `history` of every transaction touching that account.

- **Reward credit:** confirming a reward of `R` credits the student with
  `R - 1` and prepends a history entry with sender `SYSTEM`.
- **Manual transfer** debits the sender by `amount + fee`, credits the
  receiver by `amount`, increments the sender's nonce, and prepends one
  entry to each side's history with `sender, receiver, amount, fee, nonce`.
- **Validation:** the transfer is rejected if (a) the sender's balance is
  less than `amount + fee`, or (b) the submitted nonce is not exactly
  `current_nonce + 1`. The nonce check prevents replayed transactions.

History per account is browsable from the menu (option 12) by student ID.

## 5. Comparison

| Property | UTXO | Account |
|----------|------|---------|
| Balance lookup | O(n) scan of UTXO list | O(1) field read |
| Double-spend prevention | Built into the spent flag on each UTXO | Requires the nonce check |
| State size | Grows with the number of credits ever issued | Bounded by the number of accounts |
| Conceptual fit | Cash-like; each coin is traceable | Bank-account-like; one running balance |
| Implementation effort | Higher (data structure + transactions) | Lower (one field per student) |

UTXO is the right pick when traceability of each token matters; the
account model is simpler when only the net balance is interesting.

## 6. Mining Methodology

### 6.1 Solo Mining
One miner runs the proof-of-work loop on every pending block in order.
The miner is paid the full `BLOCK_REWARD = 25` per confirmed block on top
of the per-student token rewards. The number of hash attempts before a
valid hash was found is printed alongside each block.

### 6.2 Pool Mining
Three simulated miners are each assigned a random hash rate. The pool
mines each pending block (PoW runs once, the rate metric drives reward
sharing). A `pool_fee = BLOCK_REWARD * 2% = 0.50` is retained by the pool;
the remaining `24.50` is distributed according to
`share_i = (attempts_i / Σ attempts) * 24.50`. Output is a formatted table
of `Miner | Attempts | Share % | Reward` plus a total-distributed line.

### 6.3 Cloud Mining
The user rents hash power for 1–5 rounds at a configurable rental fee per
round. Each round credits `CLOUD_REWARD = 18` and debits the rental fee.
A warning is printed at any round where cumulative fees exceed cumulative
rewards. After the rental period, the rented rig confirms the pending
blocks via the same PoW loop and they are appended to the chain.

### 6.4 Difficulty
`difficulty` is the number of leading `0` characters required of the
block's SHA-256 hex hash. It is configurable from `1` to `4` via the
`--difficulty` CLI flag or menu option 2 (default `2`). Higher difficulty
multiplies expected attempts by roughly 16× per level.

## 7. Security Mechanisms

- **SHA-256 everywhere** — `tx_id`, `signature`, and `hash` are all
  SHA-256 digests of well-defined input strings.
- **Chain linkage** — every block carries `previous_hash`; option 13
  walks the chain comparing each block's `previous_hash` against the
  preceding block's `hash`, and recomputes the SHA-256 commitment.
- **Signature** — each confirmed block is committed to a system secret
  via HMAC-style SHA-256 over the signed fields. `verify_signature`
  recomputes and compares.
- **Proof of work** — the PoW hash input includes `previous_hash` and
  `signature`, so tampering with any prior block invalidates its own
  hash AND forces re-mining of every subsequent block.
- **Tamper demo** — option 14 alters a confirmed block's status in
  memory without re-signing or re-mining; option 13 then prints
  `TAMPERED: stored hash does not match contents` and `TAMPERED:
  signature does not verify`, and the change is reverted.

## 8. Edge Case Test Results

| Case | Trigger | Observed output |
|------|---------|-----------------|
| Absent → no transaction | Mark `ALU003` as `ABSENT`, mine, check balances | Balance for ALU003 stays `0`; ledger shows reward `0`; UTXO set unchanged. |
| Unknown student ID | Mark with `ALU999` | `ERROR: Student ID not found` |
| Insufficient balance (account) | Transfer `100` from `ALU001` (balance `18`) | `Rejected: insufficient account balance.` |
| Reused / wrong nonce | Transfer with `nonce = 5` when current is `0` | `Rejected: incorrect or reused nonce.` |
| Insufficient UTXOs | Transfer `100` from `ALU001` with `9` unspent | `Rejected: insufficient UTXOs for amount plus fee.` |
| Unprofitable cloud rental | Rounds `3`, rental fee `22`, reward `18` | `Warning: cloud rental is unprofitable at round 1.` (repeats per round) |
| Chain tampering | Mine three blocks, run option 14 on block 1 | `[Block 1] TAMPERED: stored hash does not match contents` and `[Block 1] TAMPERED: signature does not verify`. After revert, chain is `VALID`. |

## 9. Design Choices and Assumptions

- **Self-contained SHA-256** is bundled in the source so there is no
  OpenSSL dependency for grading. The same SHA-256 backs the `tx_id`,
  block hash, signature, and PoW.
- **HMAC-style signature** is used in place of full ECDSA to keep the
  Formative 2 binary single-file and dependency-free. The signature
  still detects field tampering and depends on a system secret, so it
  satisfies the rubric's requirement that integrity be enforced
  cryptographically.
- **In-memory state** — the pending pool, confirmed chain, UTXO set,
  and account history all live in memory for the session. The
  Formative 1 codebase handles on-disk persistence; this formative
  focuses on the mining and transaction semantics.
- **Tail-append linked lists** for the pending pool and confirmed
  chain so iteration order matches the order events occurred.
- **`previous_hash` chaining is enforced** during mining; the first
  block's `previous_hash` is `0000...0000` (the genesis hash).
- **Pool size** is hardcoded at 3 miners with random hash rates each
  round — enough to demonstrate proportional reward distribution
  without cluttering the output table.
