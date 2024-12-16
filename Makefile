# Compiler and flags for BPF programs
CLANG = clang
BPF_CFLAGS = -target bpf -g -gdwarf-4 -O2 -Wall

# Compiler and flags for user-space programs
CC = gcc
USER_CFLAGS = -g -O2 -Wall

# Directories
SRC_DIR = src
BPF_OBJ_DIR = $(SRC_DIR)

# BPF Object files
BPF_OBJ = $(BPF_OBJ_DIR)/task_register.bpf.o $(BPF_OBJ_DIR)/task_latency.bpf.o

# User-space binaries
USER_BIN = $(SRC_DIR)/task_register_user $(SRC_DIR)/task_latency_user $(SRC_DIR)/auto_task_scheduler

# Default target
all: $(BPF_OBJ) $(USER_BIN)

# Compile BPF programs
$(BPF_OBJ_DIR)/%.bpf.o: $(SRC_DIR)/%.bpf.c
	$(CLANG) $(BPF_CFLAGS) -c $< -o $@

# Compile user-space programs
$(SRC_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(USER_CFLAGS) $< -o $@ -lbpf

# Clean target
clean:
	rm -f $(BPF_OBJ) $(USER_BIN)
