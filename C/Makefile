.SILENT:

NAME = executable

CC = gcc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $@ $^
	echo "Usage: ./$(NAME) <source_directory> <replica_directory> <log_file>"
	
objs/%.o: %.c
	$(CC) -c -o $@ $<

fclean:
	rm -f $(OBJ) $(NAME)