CC=gcc
CFLAGS=-W -Wall
LYFLAGS=-ly -lfl -lm
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: yacc lex main clean run

main: $(OBJ)
	@$(CC) -o $@ $^ lex.yy.c y.tab.c $(CFLAGS) $(LYFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

yacc: *.y
	yacc -d *.y

lex: *.l
	flex *.l

mrproper: clean
	@rm analyseur

clean:
	@rm lex.* y.* *.o

run:
	./main
