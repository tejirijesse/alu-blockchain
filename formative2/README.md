# Blockchain Attendance Mining Simulation

## Compile

```bash
make
```

## Run

```bash
./attendance_mining
```

Optional arguments:

```bash
./attendance_mining --model utxo --difficulty 3
./attendance_mining --model account --difficulty 2
```

Difficulty range is `1` to `4`. The default is `2`.

## Transaction Models

The program supports two models:

- `utxo`: balances are calculated from unspent transaction outputs.
- `account`: balances are stored directly in student accounts with nonces and history logs.

You can also switch models from the menu while running the program.

## Testing Flow

1. Mark attendance for a student.
2. Print the pending pool.
3. Mine using solo, pool, or cloud mining.
4. Print balances and UTXO set.
5. Test manual transfers for insufficient balance and nonce validation.

Attendance rewards:

- PRESENT: 10 coins
- LATE: 5 coins
- ABSENT: 0 coins

Every token transaction charges a fixed 1 coin fee before crediting the receiver.
