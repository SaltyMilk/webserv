/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-melc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/07 09:04:40 by sel-melc          #+#    #+#             */
/*   Updated: 2019/10/20 12:28:12 by sel-melc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>

# define LIB_LONG_MAX 9223372036854775807L

//NEEDED TO LINK C LIB WITH C++
#ifdef __cplusplus
extern "C"{
#endif
size_t			ft_strlen(const char *str);
int				ft_isalpha(int c);
int				ft_isdigit(int c);
int				ft_isalnum(int c);
int				ft_isascii(int c);
int				ft_isprint(int c);
int				ft_toupper(int c);
int				ft_tolower(int c);
int				ft_atoi(const char *str);
int             ft_atoi_base(const char *str, const char *base);
void			*ft_memset(void *b, int c, size_t len);
void			ft_bzero(void *s, size_t n);
void			*ft_memcpy(void *dst, const void *src, size_t n);
void			*ft_memccpy(void *dst, const void *src, int c, size_t n);
void			*ft_memmove(void *dst, const void *src, size_t len);
void			*ft_memchr(const void *s, int c, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
size_t			ft_strlcpy(char *dst, const char *src, size_t size);
size_t			ft_strlcat(char *dst, const char *src, size_t size);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
char			*ft_strnstr(const char *haystack, const char *needle,
		size_t len);
int				ft_strncmp(const char *s1, const char *s2, size_t n);

void			*ft_calloc(size_t count, size_t size);
char			*ft_strdup(const char *s1);

char			*ft_substr(char const *s, unsigned int start, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strtrim(char const *s1, char const *set);
char			**ft_split(char const *s, char c);
char			*ft_itoa(int n);
char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void			ft_putchar_fd(char c, int fd);
void			ft_putstr_fd(char *s, int fd);
void			ft_putendl_fd(char *s, int fd);
void			ft_putnbr_fd(int n, int fd);

typedef struct	s_list
{
	void			*content;
	struct s_list	*next;
}				t_list;

t_list			*ft_lstnew(void *content);
void			ft_lstadd_front(t_list **alst, t_list *);
int				ft_lstsize(t_list *lst);
t_list			*ft_lstlast(t_list *lst);
void			ft_lstadd_back(t_list **alst, t_list *);
void			ft_lstdelone(t_list *lst, void(*del)(void *));
void			ft_lstclear(t_list **lst, void(*del)(void *));
void			ft_lstiter(t_list *lst, void (*f)(void *));
t_list			*ft_lstmap(t_list *lst, void *(*f)(void *),
				void (*del)(void *));

void			ft_swap(int *a, int *b);
void			ft_sort_int_tab(int *tab, int size);
void			ft_rev_int_tab(int *tab, size_t size);
int				ft_pow(int n, int pow);
int				*ft_range(int min, int max);
//ADDED FUNCTIONS
char			**ft_strtok(char *str, char *charset);
int				get_next_line(int fd, char **line);
//GNL UTILS

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

void	delol(char *tempbuffer, char mod);
char	*ultstrcat(char *s1, char *s2);
int		eolp(char *buffer, char mod);
void	fill_tempbuffer(char *tempbuffer, char *buffer, int eolp);
//END OF GNL UTILS
int							ft_satoi(const char *str);
size_t						ft_satost(const char *str);
char	*ft_strlowcase(char *str);
char	*ft_strupcase(char *str);
#ifdef __cplusplus
}
#endif
#endif
