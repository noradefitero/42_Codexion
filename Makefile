NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Isrc/include -march=native
CLINKFLAGS = -fno-semantic-interposition -fno-plt
ifdef DEBUG
    CFLAGS += -g -fdiagnostics-color=always -O0
else
    CFLAGS += -Werror -O3
endif

BUILD = build
SRCS = src/main.c \
		src/models/coder.c src/models/coder_getters.c src/models/coder_setters.c src/models/coder_th.c src/models/coder_th_lifecycle.c src/models/hub.c src/models/hub_getters.c src/models/hub_lifecycle.c src/models/usb.c \
		src/commands/commands.c  src/commands/commands_help.c \
		src/utils/time_conversion.c src/utils/io_wrappers.c src/utils/ft/ft_isalpha.c src/utils/ft/ft_strisdigit.c src/utils/ft/ft_calloc.c
OBJS = $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(CLINKFLAGS) $(OBJS) -o $(NAME)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re