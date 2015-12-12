CC=gcc
CFLAGS=-g -W -Wall
LYFLAGS=-ly -lfl -lm
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: yacc lex main remove_obj_only run exec

exec:
	spim -f out.s

all_noclean: yacc lex main run

remove_obj_only:
	@rm lex.* y.* *.o

main: $(OBJ)
	@$(CC) -o $@ $^ lex.yy.c y.tab.c $(CFLAGS) $(LYFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

yacc: *.y
	yacc -d --verbose matc.y

lex: *.l
	flex matc.l

mrproper: clean
	@rm analyseur

clean: remove_obj_only
	@rm main *.s

run:
	./main
