# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hlabouit <hlabouit@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/31 05:26:15 by hlabouit          #+#    #+#              #
#    Updated: 2024/10/03 14:51:48 by hlabouit         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
BONUS_NAME = ircserv_bonus

CPPFILES = Server/Server.cpp main.cpp Server/authentication.cpp Server/client_data_erasure.cpp \
			Server/processing_polled_data.cpp Server/useful_methods.cpp Channel.cpp \
			Client.cpp  CMD/Join.cpp CMD/Privmsg.cpp CMD/Invite.cpp CMD/Topic.cpp CMD/Kick.cpp \
			CMD/Part.cpp CMD/Quit.cpp CMD/Mode.cpp \
			
CPPOFILES = $(CPPFILES:%.cpp=%.o)

BONUS_CPPFILES = bonus/Bot.cpp bonus/main_bonus.cpp
BONUS_CPPOFILES = $(BONUS_CPPFILES:%.cpp=%.o)

CPPC = c++ -g
CPPFLAGS =  -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g

HEADERS = includes/Server.hpp includes/Channel.hpp includes/Client.hpp includes/replies.hpp

all: $(NAME)

$(NAME): $(CPPOFILES)
	@$(CPPC) $(CPPOFILES) -o $(NAME)

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(CPPOFILES) $(BONUS_CPPOFILES)
	@$(CPPC) $(CPPOFILES) $(BONUS_CPPOFILES) -o $(BONUS_NAME)

%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@$(CPPC) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -rf $(CPPOFILES) $(BONUS_CPPOFILES)

fclean: clean
	@rm -rf $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all clean fclean re bonus
