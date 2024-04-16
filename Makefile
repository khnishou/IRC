SRC = src/Server.cpp src/main.cpp src/Users.cpp src/Channel.cpp lib/skip.cpp lib/initMode.cpp lib/split.cpp
OBJ = $(SRC:.cpp=.o)
CC = g++
RM = rm -rf
CPPFLAGS = -std=c++98 -pedantic

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re