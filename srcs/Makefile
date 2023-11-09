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
		
TEST_SRCS = tests/test.cpp \
			tests/CAP_test.cpp \
			tests/INVITE_test.cpp \
			tests/JOIN_test.cpp \
			tests/KICK_test.cpp \
			tests/MODE_test.cpp \
			tests/NICK_test.cpp \
			tests/PART_test.cpp \
			tests/PASS_test.cpp \
			tests/PING_test.cpp \
			tests/PRIVMSG_test.cpp \
			tests/QUIT_test.cpp \
			tests/TOPIC_test.cpp \
			tests/USER_test.cpp \
			tests/WHOIS_test.cpp \

OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all : $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)   -c -o $@ $<

$(NAME): $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS)      -o $(NAME) $(OBJS) $(TEST_OBJS)

run:		all
			./$(NAME)

clean :
	rm -f $(OBJS) $(TEST_OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re