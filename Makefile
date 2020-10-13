NAME = webserv

SRC_DIR = srcs/

SRC = net.cpp

CODES_DIR = codes
CODES_FILES = status_code.cpp status_code_utils.cpp

CONFIG_DIR = config
CONFIG_FILES = parse_config.cpp parse_config_utils.cpp

REQUEST_DIR = request
REQUEST_FILES = parse_request.cpp

RESPONSE_DIR = response
RESPONSE_FILES = response.cpp response_utils.cpp

UTILS_DIR = utils
UTILS_FILES = date.cpp utils.cpp content_type.cpp

SRC += $(addprefix $(CODES_DIR)/, $(CODES_FILES))
SRC += $(addprefix $(CONFIG_DIR)/, $(CONFIG_FILES))
SRC += $(addprefix $(REQUEST_DIR)/, $(REQUEST_FILES))
SRC += $(addprefix $(RESPONSE_DIR)/, $(RESPONSE_FILES))
SRC += $(addprefix $(UTILS_DIR)/, $(UTILS_FILES))

SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJ = $(SRCS:.cpp=.o)

CC = clang++
CFLAGS = -Wall -Wextra -Werror -Iincludes -lft -L. -fsanitize=address

all: libft $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(SRCS)

libft:
	make -C ./libft
	cp ./libft/libft.a ./libft.a
clean:
	make -C ./libft clean
	rm -rf libft.a
fclean:
	make -C ./libft fclean
	rm -rf $(OBJ)
	rm -rf libft.a
	rm -rf webserv
re: fclean all


.PHONY: libft all $(NAME)