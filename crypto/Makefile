CC = gcc
AR = ar
CFLAGS = -Wall -Werror -Wextra -pedantic
SRC = sha256.c ec_create.c ec_to_pub.c ec_from_pub.c ec_save.c ec_load.c \
	ec_sign.c ec_verify.c
OBJ = $(SRC:.c=.o)
NAME = libhblk_crypto.a

.PHONY: all clean oclean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) rcs $(NAME) $(OBJ)

%.o: %.c hblk_crypto.h
	$(CC) $(CFLAGS) -c $< -o $@

clean: oclean

oclean:
	$(RM) $(OBJ)

fclean: oclean
	$(RM) $(NAME)

re: fclean all
