NAME = webserv

SRC_DIR = srcs/

SRC = net.cpp

CGI_DIR = cgi
CGI_FILES = cgi.cpp environment.cpp cgi_utils.cpp

CODES_DIR = codes
CODES_FILES = status_code.cpp status_code_utils.cpp

CONFIG_DIR = config
CONFIG_FILES = parse_config.cpp parse_config_utils.cpp

REQUEST_DIR = request
REQUEST_FILES = parse_request.cpp parse_request_utils.cpp headers.cpp

RESPONSE_DIR = response
RESPONSE_FILES = response.cpp response_utils.cpp

UTILS_DIR = utils
UTILS_FILES = date.cpp utils.cpp content_type.cpp signals.cpp base64.cpp

SRC += $(addprefix $(CGI_DIR)/, $(CGI_FILES))
SRC += $(addprefix $(CODES_DIR)/, $(CODES_FILES))
SRC += $(addprefix $(CONFIG_DIR)/, $(CONFIG_FILES))
SRC += $(addprefix $(REQUEST_DIR)/, $(REQUEST_FILES))
SRC += $(addprefix $(RESPONSE_DIR)/, $(RESPONSE_FILES))
SRC += $(addprefix $(UTILS_DIR)/, $(UTILS_FILES))

SRCS = $(addprefix $(SRC_DIR), $(SRC))
OBJ = $(SRCS:.cpp=.o)

CC = clang++
CFLAGS = -Wall -Wextra -Werror
LIB = -Llibft -lft
INC = includes
INC_LIB = libft

ccblue = "\33[0;34m"
ccred = "\033[0;91m"
ccgreen = "\033[0;92m"
ccgreenhard = "\033[0;32m"
cccyan = "\033[0;96m"
ccreset = "\033[0;0m"
cclightgray = "\033[0;37m"

all: libft $(NAME)

$(NAME): $(OBJ)
	@printf $(cccyan)
	@printf "Compiling $(NAME) "
	@$(CC) $(CFLAGS) $(LIB) -I$(INC) -o $(NAME) $(OBJ) -fsanitize=address
	@printf $(cclightgray)[$(ccgreenhard)√$(cclightgray)]$(ccreset)
	@printf "                                                     \n"

%.o: %.cpp
	@printf $(ccblue)
	@printf "Compiling $(notdir $<) "
	@$(CC) $(CFLAGS) -I$(INC) -I$(INC_LIB) -o $@ -c $<
	@printf $(cclightgray)[$(ccgreenhard)√$(cclightgray)]$(ccreset)
	@printf "\n"

libft:
	@make -C ./libft

clean:
	@make -C ./libft clean
	@rm -rf $(OBJ)

fclean:
	@make -C ./libft fclean
	@rm -rf $(OBJ)
	@rm -rf $(NAME)

re: fclean all

.PHONY: libft all $(NAME)