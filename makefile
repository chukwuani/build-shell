CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lreadline

SRCS = main.c utils/utils.c builtins/builtins.c
OBJS = $(SRCS:.c=.o)

shell: $(OBJS)
	$(CC) $(CFLAGS) -o shell.exe $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: shell
	./shell.exe

clean:
	rm -f $(OBJS) shell.exe