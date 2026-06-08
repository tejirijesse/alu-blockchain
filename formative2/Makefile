CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99
NAME = attendance_mining
SRC = attendance_mining.c

.PHONY: all clean

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(NAME)
