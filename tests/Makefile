# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dde-fite <dde-fite@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/17 19:54:42 by dde-fite          #+#    #+#              #
#    Updated: 2026/09/18 04:23:25 by dde-fite         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ROOT = ..

CC = cc
CFLAGS = -Wall -Wextra -g -O0 -pthread -I$(ROOT)/src/include \
	-I$(ROOT)/tests/Unity/src

ifdef SANITIZE
	CFLAGS += -fsanitize=address,undefined -O1 -fno-omit-frame-pointer
	OBJ_ROOT = $(ROOT)/build/tests/obj/san
	BIN_DIR =  $(ROOT)/build/tests/bin/san
else
	OBJ_ROOT = $(ROOT)/build/tests/obj
	BIN_DIR = $(ROOT)/build/tests/bin
endif

COMMON_OBJ_DIR = $(OBJ_ROOT)/common

SRCS = \
	src/commands/commands.c \
	src/commands/commands_help.c \
	src/io/parsing_params.c \
	src/io/print.c \
	src/models/coder.c \
	src/models/coder_setters.c \
	src/models/coder_th.c \
	src/models/coder_th_usb.c \
	src/models/coder_th_lifecycle.c \
	src/models/hub.c \
	src/models/hub_events.c \
	src/models/hub_lifecycle.c \
	src/models/hub_threads.c \
	src/models/logger.c \
	src/models/logger_queue.c \
	src/models/logger_th.c \
	src/models/monitor.c \
	src/models/monitor_th.c \
	src/models/schedulers/fifo.c \
	src/models/schedulers/fifo_queue.c \
	src/models/schedulers/edf.c \
	src/models/schedulers/edf_queue.c \
	src/models/schedulers/edf_ops.c \
	src/models/usb.c \
	src/models/usb_api.c \
	src/utils/ft/ft_calloc.c \
	src/utils/ft/ft_isalpha.c \
	src/utils/ft/ft_strisdigit.c \
	src/utils/ft/ft_memcpy.c \
	src/utils/ft/ft_realloc.c \
	src/utils/ft/ft_minsize.c \
	src/utils/time.c

COMMON_OBJS = $(patsubst src/%.c,$(COMMON_OBJ_DIR)/%.o,$(SRCS))
UNITY_OBJ = $(OBJ_ROOT)/unity/unity.o

BINS = test_utils test_sched test_io test_models test_program

# The compiled codexion binary exercised by the end-to-end suite. The path is
# made absolute so the forked child can find it from any cwd.
CODEXION_BIN = $(abspath $(ROOT)/codexion)
CODEXION_SRCS = $(addprefix $(ROOT)/, $(SRCS))

all: $(patsubst %,$(BIN_DIR)/%,$(BINS))
	$(MAKE) -C $(ROOT) all

$(COMMON_OBJ_DIR)/%.o: $(ROOT)/src/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(UNITY_OBJ): $(ROOT)/tests/Unity/src/unity.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_ROOT)/%.o: $(ROOT)/tests/unit/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -I$(ROOT)/tests/unit $< -o $@

# test_binary needs the absolute path of the compiled program as a string
# literal; the specific rule overrides the generic %.o pattern rule above.
$(OBJ_ROOT)/test_binary.o: $(ROOT)/tests/unit/test_binary.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -I$(ROOT)/tests/unit \
		-DCODEXION_BIN='"$(CODEXION_BIN)"' $< -o $@

$(BIN_DIR)/test_utils: $(OBJ_ROOT)/test_ft.o $(OBJ_ROOT)/test_time.o \
	$(OBJ_ROOT)/capture.o $(OBJ_ROOT)/runner_utils.o $(UNITY_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_sched: $(OBJ_ROOT)/test_fifo.o $(OBJ_ROOT)/test_edf.o \
	$(OBJ_ROOT)/test_usb.o $(OBJ_ROOT)/capture.o $(OBJ_ROOT)/runner_sched.o \
	$(UNITY_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_io: $(OBJ_ROOT)/test_parsing.o $(OBJ_ROOT)/test_print.o \
	$(OBJ_ROOT)/test_commands.o $(OBJ_ROOT)/test_logger.o \
	$(OBJ_ROOT)/capture.o $(OBJ_ROOT)/runner_io.o $(UNITY_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_models: $(OBJ_ROOT)/test_coder.o $(OBJ_ROOT)/test_monitor.o \
	$(OBJ_ROOT)/test_hub.o $(OBJ_ROOT)/capture.o $(OBJ_ROOT)/runner_models.o \
	$(UNITY_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_program: $(OBJ_ROOT)/test_binary.o $(OBJ_ROOT)/runner_program.o \
	$(OBJ_ROOT)/capture.o $(UNITY_OBJ) $(COMMON_OBJS) | $(CODEXION_BIN)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

# The end-to-end suite runs the compiled program itself; make sure it exists
# and is up to date (the recipe delegates freshness to the root Makefile).
$(CODEXION_BIN): $(CODEXION_SRCS)

objs: $(COMMON_OBJS) $(UNITY_OBJ)

test_utils: $(BIN_DIR)/test_utils
test_sched: $(BIN_DIR)/test_sched
test_io: $(BIN_DIR)/test_io
test_models: $(BIN_DIR)/test_models
test_program: $(BIN_DIR)/test_program


run: all
	@status=0; \
	for t in $(BINS); do \
		echo "==== $$t ===="; \
		$(BIN_DIR)/$$t || { \
			echo "ERROR: unit test $$t FAILED"; \
			status=1; \
		}; \
	done; \
	if [ $$status -eq 0 ]; then \
		echo "\nALL UNIT TESTS PASSED"; \
	else \
		echo "\nSOME UNIT TESTS FAILED"; \
	fi; \
	exit $$status

run_sanitize:
	$(MAKE) run SANITIZE=1

run_valgrind: all
	@status=0; \
	for t in $(BINS); do \
		echo "==== $$t ===="; \
		valgrind --leak-check=full --error-exitcode=1 $(BIN_DIR)/$$t || { \
			echo "ERROR: valgrind detected errors in $$t"; \
			status=1; \
		}; \
	done; \
	if [ $$status -eq 0 ]; then \
		echo "\nALL UNIT TESTS + VALGRIND PASSED"; \
	else \
		echo "\nSOME UNIT TESTS + VALGRIND FAILED"; \
	fi; \
	exit $$status

clean:
	rm -rf $(ROOT)/build/tests

.PHONY: all test_utils test_sched test_io test_models test_program run run_sanitize clean
