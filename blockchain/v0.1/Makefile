CC = gcc
AR = ar
CFLAGS = -Wall -Werror -Wextra -pedantic
SRC = blockchain_create.c block_create.c block_destroy.c blockchain_destroy.c \
	block_hash.c blockchain_serialize.c blockchain_deserialize.c \
	block_is_valid.c provided/_endianness.c
OBJ = $(SRC:.c=.o)
NAME = libhblk_blockchain_v0.1.a

.PHONY: all clean oclean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) rcs $(NAME) $(OBJ)

%.o: %.c blockchain.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean: oclean

oclean:
	$(RM) $(OBJ)

fclean: oclean
	$(RM) $(NAME)

re: fclean all
