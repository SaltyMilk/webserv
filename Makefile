NAME = webserv
SRC =	net.cpp utils.cpp parse_request.cpp response.cpp date.cpp parse_config.cpp \
		content_type.cpp status_code.cpp response_utils.cpp parse_config_utils.cpp
CC = clang++
CFLAGS = -Wall -Wextra -Werror -lft -L. -fsanitize=address

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