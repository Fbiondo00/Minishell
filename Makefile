NAME = minishell

SRC       = main.c
LEXER	  = $(wildcard lexer/*.c)
EXECUTOR  = $(wildcard executor/*.c) $(wildcard executor/*/*.c)
PARSER    = $(wildcard parser/*.c)
SIGNAL    = $(wildcard signal/*.c)
UTILS    =  $(wildcard utils/*.c)

OBJS  = ${SRC:.c=.o} $(PARSER:%.c=%.o) $(LEXER:%.c=%.o) $(EXECUTOR:%.c=%.o) $(SIGNAL:%.c=%.o) $(UTILS:%.c=%.o)

FLAGS = -Wextra -Werror -Wall
CC    = @gcc -g3 ${FLAGS} 
RM    = @rm -f

RED		=	\033[31m
GREEN	=	\033[32m
BLUE	=	\033[34m
CYAN	=	\033[36m
BOLD	=	\033[1m
UNDER	=	\033[4m
END		=	\033[0m
RESET	= \033[0;0m

SRC_COUNT     = 0
SRC_COUNT_TOT = 61
SRC_PCT       = $(shell expr 100 \* $(SRC_COUNT) / $(SRC_COUNT_TOT))

%.o : %.c
	@$(CC) $(FLAGS) -c $< -o $@
	@$(eval SRC_COUNT = $(shell expr $(SRC_COUNT) + 1))
	@printf "$(BOLD)$(GREEN)\r%100s\r[%d/%d] 😊(%d%%) $(END)$(BOLD)$(BLUE)$<" "" $(SRC_COUNT) $(SRC_COUNT_TOT) $(SRC_PCT)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -lreadline -o $(NAME) 
	@printf "$(UNDER)$(BOLD)$(CYAN)\nCompiling minishell\n$(RESET)$(BOLD)"

all: $(NAME)

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}
	@printf "$(UNDER)$(BOLD)$(RED)Removing minishell$(RESET)$(RED$)$(BOLD) .......$(RESET)"
	
re: fclean all

.PHONY: all clean fclean re