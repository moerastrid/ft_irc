NAME := ircserver
FLAGS =  -Wall -Wextra -Werror -std=c++20 -fsanitize=address
CXX = c++

HDR = ircserver.hpp \
	Msg.hpp \
	Server.hpp

SRC = main.cpp \
	parse.cpp \
	Msg.cpp \
	Server.cpp

OBJ_DIR = obj

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all : $(NAME)

$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	$(CXX) $(FLAGS) -c -o $@ $<

$(NAME): $(OBJ) $(HDR)
	$(CXX) $(FLAGS)	-o $(NAME) $(OBJ)

$(OBJ_DIR) :
	mkdir -pv $(OBJ_DIR)

run:	all
	./$(NAME)

clean :
	rm -fr $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
