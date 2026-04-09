CC = gcc
SRC = main.c utils/utils.c
OUT = shell.exe

all:
	$(CC) $(SRC) -o $(OUT)

clean:
	del $(OUT)