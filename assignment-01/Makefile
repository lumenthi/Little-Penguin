# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/12/22 14:06:43 by lumenthi          #+#    #+#              #
#    Updated: 2022/09/17 14:55:01 by lumenthi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.PHONY : all clean fclean re test

NAME = main

KERNEL = $(HOSTTYPE)

ifeq ($(KERNEL),)
	KERNEL := $(shell uname -r)
endif

GREEN = '\033[4;32m'
RED = '\033[4;31m'
BLANK = '\033[0m'
YELLOW = '\033[4;33m'
CYAN = '\033[4;38;5;51m'

TICK = '\033[1;32m~\033[0m'
CROSS = '\033[1;31mx\033[0m'

MODULE = $(NAME).ko
SOURCE = $(NAME).c
OBJECT = $(NAME).o

obj-m:=$(OBJECT)

all: $(NAME)

$(NAME):
	@ make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules
	@ printf " %b | Compiled %b%b%b\n" $(TICK) $(GREEN) $(NAME) $(BLANK)

clean:
	@ cp $(MODULE) /tmp 2>/dev/null | true
	@ make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean
	@ mv /tmp/$(MODULE) ./ 2>/dev/null | true

fclean: clean
	@ rm $(MODULE)

re: fclean all

test: all
	@ echo "Installing $(MODULE)"
	@ sudo insmod $(MODULE)
	@ sudo dmesg | tail -1
	@ echo "Removing $(MODULE)"
	@ sudo rmmod $(MODULE)
	@ sudo dmesg | tail -1
