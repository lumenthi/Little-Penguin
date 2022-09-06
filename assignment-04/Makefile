# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lumenthi <lumenthi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/12/22 14:06:43 by lumenthi          #+#    #+#              #
#    Updated: 2022/09/06 12:48:01 by lumenthi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.PHONY : all clean fclean re install uninstall

NAME = kbd
RULE = 01-usb-keyboard.rules

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

install: $(NAME)
	@ sudo cp -v $(RULE) /etc/udev/rules.d/$(RULE)
	@ sudo udevadm control --reload-rules
	@ sudo mkdir -pv /usr/lib/modules
	@ sudo cp -v $(MODULE) /usr/lib/modules/$(MODULE)

uninstall: fclean
	@ sudo rm -v /etc/udev/rules.d/$(RULE)
	@ sudo rm -v /usr/lib/modules/$(MODULE)
	@ sudo rmmod $(NAME)
