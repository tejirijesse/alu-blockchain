# Technical Report: Blockchain Transaction Models and Mining Simulation

## Attendance to Transactions

When a student is marked present or late, the system creates a pending attendance block and a token reward transaction. Present earns 10 coins and late earns 5 coins. Absent creates no token transaction. The attendance block is placed in a pending pool until mining confirms it.

## Pending Pool and Mining

Attendance records are not immediately permanent. Mining performs proof of work by changing a nonce until the block hash starts with the required number of zero characters. Once mining succeeds, the block is moved from the pending pool to the confirmed chain and the token transaction updates balances.

## UTXO Model

The UTXO model stores balances as unspent transaction outputs. A student balance is calculated by summing all unspent outputs owned by that student. Each transaction deducts a 1 coin fee. If inputs exceed the output plus fee, the remainder is returned to the sender as a change UTXO. Spent UTXOs cannot be reused, preventing double spending.

## Account Model

The account model stores one balance per student. Each student account has a nonce that increments for every outgoing transfer. A transaction with a reused or incorrect nonce is rejected. Each account keeps an in-memory linked-list history containing sender, recipient, amount, fee, and nonce.

## Model Comparison

UTXO is good for tracking exact spendable outputs and preventing double spending by marking outputs as spent. The account model is simpler for balances but needs nonce checks to prevent replayed transactions.

## Mining Methods

Solo mining gives one miner the full reward after proof of work. Pool mining assigns random hash rates to miners and splits rewards according to contribution after a 2% pool fee. Cloud mining simulates renting mining power for 1 to 5 rounds and prints gross earnings, fees, and net profit, warning when fees exceed rewards.

## Edge Cases

- ABSENT attendance creates no token transaction.
- UTXO transfers fail if inputs cannot cover amount plus fee.
- Account transfers fail if the balance is insufficient.
- Account transfers fail if the nonce is reused or incorrect.
- Cloud mining warns when cumulative fees exceed rewards.
