NAME = webserv
SRC = net.cpp utils.cpp parse_request.cpp
CC = clang++
CFLAGS = -fsanitize=address -Wall -Wextra -Werror -lft -L.

all: libft $(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) -o $(NAME) $(SRC)

libft:
	make -C ./libft
	cp ./libft/libft.a ./libft.a
clean:
	make -C ./libft clean
	rm -rf libft.a
fclean:
	make -C ./libft fclean
	rm -rf libft.a
	rm -rf webserv
re: fclean all


.PHONY: libft all $(NAME)