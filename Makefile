
NAME = ircserv
CC = g++
RM = rm -rf
CFLAGS =  -std=c++98 -MD

SRC		=	Server main Users Channel Utils ServerCommands ServerEngine

SRC_DIR = src/
OBJ_DIR = objects/

SRCS = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC)))

OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC)))

all: $(NAME) 

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) 

re: fclean all
.PHONY: all clean fclean re