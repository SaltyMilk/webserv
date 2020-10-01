# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/10/15 19:41:09 by sel-melc          #+#    #+#              #
#    Updated: 2019/10/21 22:02:50 by sel-melc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MSRC = ft_atoi.c ft_bzero.c ft_calloc.c ft_isalnum.c ft_isalpha.c ft_isascii.c ft_isdigit.c \
	  ft_isprint.c ft_itoa.c ft_memccpy.c ft_memchr.c ft_memcmp.c ft_memcpy.c ft_memmove.c \
	  ft_memset.c ft_putchar_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_putstr_fd.c ft_split.c \
	  ft_strchr.c ft_strdup.c ft_strjoin.c ft_strlcat.c ft_strlcpy.c ft_strlen.c ft_strmapi.c \
	  ft_strncmp.c ft_strnstr.c ft_strrchr.c ft_strtrim.c ft_substr.c ft_tolower.c ft_toupper.c\
	  ft_strtok.c get_next_line_bonus.c get_next_line_utils_bonus.c ft_satoi.c ft_satost.c
BSRC = ft_lstadd_back_bonus.c ft_lstadd_front_bonus.c ft_lstclear_bonus.c ft_lstdelone_bonus.c \
	   ft_lstiter_bonus.c ft_lstlast_bonus.c ft_lstmap_bonus.c ft_lstnew_bonus.c ft_lstsize_bonus.c \
	   ft_rev_int_tab_bonus.c ft_sort_int_tab_bonus.c ft_swap_bonus.c ft_pow_bonus.c ft_range_bonus.c
CC = gcc
CFLAGS = -c -Wall -Wextra -Werror
MOBJ = $(MSRC:.c=.o)
BOBJ = $(BSRC:.c=.o)

NAME = libft.a

all : $(NAME)

$(NAME) : $(MOBJ)
	ar -rc $(NAME) $(MOBJ)

%.o: %.c
	$(CC) $(CFLAGS) $?

bonus : $(MOBJ) $(BOBJ)
	ar -rc $(NAME) $(MOBJ) $(BOBJ)

clean : 
	rm -f $(MOBJ) $(BOBJ)
fclean :
	rm -f $(MOBJ) $(BOBJ) $(NAME)

re : fclean all
reb : fclean bonus

.PHONY : clean fclean re reb bonus all
