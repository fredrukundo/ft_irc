# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/31 05:26:15 by hlabouit          #+#    #+#              #
#    Updated: 2024/08/30 16:34:28 by frukundo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc_server

CPPFILES = Server.cpp main.cpp Channel.cpp Client.cpp Authenti.cpp CMD/Join.cpp CMD/PRIVMSG.cpp

CPPOFILES = $(CPPFILES:%.cpp=%.o)

CPPC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g 

HEADERS = Server.hpp ./includes

all: $(NAME)

$(NAME): $(CPPOFILES) $(HEADERS)
	@$(CPPC) $(CPPOFILES) -o $(NAME)

%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CPPC) $(CPPFLAGS) -c $< -o $@


clean:
	@rm -rf $(CPPOFILES)

fclean: clean
	@rm -rf $(NAME)

re: fclean all 

.PHONY: all clean fclean re

