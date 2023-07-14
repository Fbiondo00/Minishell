NAME = minishell

SRC			= main.c
LEXER	= $(wildcard lexer/*.c)
PARSER	= $(wildcard executor/*.c)
EXECUTOR	= $(wildcard parser/*.c)

BLUE	= \033[1;34m
RED		= \033[0;31m
GREEN	= \033[0;32m
RESET	= \033[0;0m
ECHO	=	@echo

FLAGS = -Wextra -Werror -Wall
CC = @gcc ${FLAGS} 
CMP = $(ECHO) "\nCompiling minishell"
RM = @rm -f

OBJS = ${SRC:.c=.o} $(PARSER:%.c=%.o) $(LEXER:%.c=%.o) $(EXECUTOR:%.c=%.o)

SRC_COUNT = 0
SRC_COUNT_TOT = $(shell expr $(shell echo -n $(SRC) $(PARSER) $(LEXER) $(EXECUTOR) | wc -w))
SRC_PCT = $(shell expr 100 \* $(SRC_COUNT) / 7)
%.o : %.c

	@$(CC) $(FLAGS) -c $< -o $@
	@$(eval SRC_COUNT = $(shell expr $(SRC_COUNT) + 1))
	@printf "$(GREEN)\r%100s\r[%d/%d (%d%%)] $(GREEN)$<" "" $(SRC_COUNT) 7 $(SRC_PCT)


$(NAME): $(OBJS)
	$(CC) $(OBJS) -lreadline -o $(NAME) 
	$(CMP)

all: $(NAME)

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}
	@printf "$(RED)Removing minishell .......$(RESET)"
	
re: fclean all
	$(CMP)

.PHONY: all clean fclean re