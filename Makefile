NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g 
SRC = parsing.c thread.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)



# PTHREAD   = -pthread ?
# HEADER    = header.h

# .PHONY: all clean fclean re