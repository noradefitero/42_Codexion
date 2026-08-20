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
SRCS = 	src/main.c \
		src/models/coder.c \
		src/models/coder_setters.c \
		src/models/coder_th.c \
		src/models/coder_th_lifecycle.c \
		src/models/hub.c \
		src/models/hub_lifecycle.c \
		src/models/hub_events.c \
		src/models/usb.c \
		src/models/monitor.c \
		src/models/monitor_th.c \
		src/models/logger.c \
		src/models/logger_th.c \
		src/models/logger_queue.c \
		src/commands/commands.c \
		src/commands/commands_help.c \
		src/io/print.c \
		src/io/parsing_params.c \
		src/utils/time.c \
		src/utils/ft/ft_isalpha.c \
		src/utils/ft/ft_strisdigit.c \
		src/utils/ft/ft_calloc.c
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