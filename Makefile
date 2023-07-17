NAME = minishell

SRC			= main.c
LEXER	= $(wildcard lexer/*.c)
PARSER	= $(wildcard executor/*.c)
EXECUTOR	= $(wildcard parser/*.c)
SIGNAL = $(wildcard signal/*.c)

ERASE	=	\033[2K\r
GREY	=	\033[30m
RED		=	\033[31m
GREEN	=	\033[32m
YELLOW	=	\033[33m
BLUE	=	\033[34m
PINK	=	\033[35m
CYAN	=	\033[36m
WHITE	=	\033[37m
BOLD	=	\033[1m
UNDER	=	\033[4m
SUR		=	\033[7m
END		=	\033[0m
CENTER	=	\033[60G
RESET	= \033[0;0m
ECHO	=	@echo

FLAGS = -Wextra -Werror -Wall
CC = @gcc ${FLAGS} 
RM = @rm -f

OBJS = ${SRC:.c=.o} $(PARSER:%.c=%.o) $(LEXER:%.c=%.o) $(EXECUTOR:%.c=%.o)

SRC_COUNT = 0
SRC_COUNT_TOT = 9
SRC_PCT = $(shell expr 100 \* $(SRC_COUNT) / 9)

%.o : %.c
	@$(CC) $(FLAGS) -c $< -o $@
	@$(eval SRC_COUNT = $(shell expr $(SRC_COUNT) + 1))
	@printf "$(BOLD)$(GREEN)\r%100s\r[%d/%d] 😊(%d%%) $(END)$(BOLD)$(BLUE)$<" "" $(SRC_COUNT) $(SRC_COUNT_TOT) $(SRC_PCT)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -lreadline -o $(NAME) 
	@printf "$(UNDER)$(BOLD)$(CYAN)\nCompiling minishell\n$(RESET)$(BOLD)"
	@./minishell

all: $(NAME)

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}
	@printf "$(UNDER)$(BOLD)$(RED)Removing minishell$(RESET)$(RED$)$(BOLD) .......$(RESET)"
	
re: fclean all


.PHONY: all clean fclean re