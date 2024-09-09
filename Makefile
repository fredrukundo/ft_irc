# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: frukundo <frukundo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/31 05:26:15 by hlabouit          #+#    #+#              #
#    Updated: 2024/09/08 17:59:29 by frukundo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc_server

CPPFILES = Server.cpp main.cpp Channel.cpp Client.cpp CMD/Join.cpp CMD/Privmsg.cpp CMD/Invite.cpp CMD/Topic.cpp CMD/Kick.cpp CMD/Part.cpp CMD/Quit.cpp CMD/Mode.cpp
CPPOFILES = $(CPPFILES:%.cpp=%.o)

CPPC = c++ -g
CPPFLAGS =  -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g 

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
