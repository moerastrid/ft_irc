# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tnuyten <tnuyten@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/12 20:18:34 by tnuyten           #+#    #+#              #
#    Updated: 2023/09/15 19:55:04 by tnuyten          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := server_backend
CXXFLAGS =  -Wall -Wextra -Werror
CXXFLAGS += -fsanitize=address -g
CXX = c++

SRCS =	Channel.cpp \
		Client.cpp \
		Command.cpp \
		Executor.cpp \
		Executor_mode.cpp \
		test.cpp

OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)   -c -o $@ $<

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS)      -o $(NAME) $(OBJS)

run:		all
			./$(NAME)

clean :
	rm -f $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re