.PHONY: all clean
CC = gcc
SRC = $(wildcard *.c)
FLAGS = -Wall -Werror -Wextra -pedantic -g -pthread
BCHAIN = -lhblk_blockchain -L../blockchain/v0.3 -I../blockchain/v0.3
CRY = -lhblk_crypto -L../crypto -I../crypto
HEX = hex/hex.c
LINKER = -lcrypto -lllist -lssl
NAME = cli
BGEN = ../blockchain/v0.3/provided/_genesis.c



#default
all:
	$(CC) $(SRC) $(BGEN) $(FLAGS) $(BCHAIN) $(CRY) $(LINKER) $(HEX) -o $(NAME)

#clean
clean:
	rm -f $(NAME)
