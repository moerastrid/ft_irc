NAME := ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -Iinclude
CXXFLAGS += -g
# CXXFLAGS += -g -fsanitize=address
MAKEFLAGS += --no-print-directory

SRC_DIR = src
OBJ_DIR = obj

HDR = include/ircserver.hpp \
	include/Msg.hpp \
	include/Server.hpp \
	include/Channel.hpp \
	include/Client.hpp \
	include/Command.hpp \
	include/Env.hpp \
	include/Executor.hpp \
	include/SignalHandler.hpp

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/parse.cpp \
	$(SRC_DIR)/Msg.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Command.cpp \
	$(SRC_DIR)/Env.cpp \
	$(SRC_DIR)/Executor.cpp \
	$(SRC_DIR)/Executor_mode.cpp \
	$(SRC_DIR)/SignalHandler.cpp \


OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

# all : $(NAME)
all:
	$(MAKE) -j $(NAME)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(NAME): $(OBJ) $(HDR)
	$(CXX) $(CXXFLAGS)	-o $(NAME) $(OBJ)

$(OBJ_DIR) :
	mkdir -pv $(OBJ_DIR)

clean :
	rm -fr $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
