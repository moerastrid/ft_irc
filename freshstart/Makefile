NAME := ircserver
FLAGS =  -Wall -Wextra -Werror -Iinclude -fsanitize=address #-std=c++20
CXX = c++

SRC_DIR = src
OBJ_DIR = obj

HDR = include/ircserver.hpp \
	include/Msg.hpp \
	include/Server.hpp

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/parse.cpp \
	$(SRC_DIR)/Msg.cpp \
	$(SRC_DIR)/Server.cpp

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all : $(NAME)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
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
