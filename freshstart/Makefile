NAME := ircserver
FLAGS =  -Wall -Wextra -Werror -Iinclude -g -fsanitize=address #-std=c++20
CXX = c++

SRC_DIR = src
OBJ_DIR = obj

HDR = include/ircserver.hpp \
	include/Msg.hpp \
	include/Server.hpp \
	include/Channel.hpp \
	include/Client.hpp \
	include/Command.hpp \
	include/Env.hpp \
	include/Executor.hpp

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/parse.cpp \
	$(SRC_DIR)/Msg.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Command.cpp \
	$(SRC_DIR)/Env.cpp \
	$(SRC_DIR)/Executor.cpp \
	$(SRC_DIR)/Executor_mode.cpp


OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all : $(NAME)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(FLAGS) -c -o $@ $<

$(NAME): $(OBJ) $(HDR)
	$(CXX) $(FLAGS)	-o $(NAME) $(OBJ)

$(OBJ_DIR) :
	mkdir -pv $(OBJ_DIR)

clean :
	rm -fr $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : fclean all
# re :
# 	$(MAKE) -j fclean all

.PHONY: all clean fclean re
