NAME := ft_irc
FLAGS =  -Wall -Wextra -Werror -std=c++20
CXX = c++

HDR = ft_irc.hpp

SRC = main.cpp \
	ft_irc.cpp

OBJ_DIR = obj

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all : $(NAME)

$(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	$(CXX) $(FLAGS) -c -o $@ $<

$(NAME): $(OBJ) $(HDR)
	$(CXX) $(FLAGS)	-o $(NAME) $(OBJ)

$(OBJ_DIR) :
	mkdir -pv $(OBJ_DIR)

run:		all
			./$(NAME)

clean :
	rm -fr $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
