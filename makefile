CC=gcc
CFLAGS=-g -W -Wall
LYFLAGS=-ly -lfl -lm
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: yacc lex main remove_obj_only

all_exec: yacc lex main remove_obj_only run_test exec

all_mmcheck: yacc lex main remove_obj_only run_test_mmcheck

exec:
	spim -f out.s

all_noclean: yacc lex main run

remove_obj_only:
	@rm lex.* y.t* *.o

main: $(OBJ)
	@$(CC) -o $@ $^ lex.yy.c y.tab.c $(CFLAGS) $(LYFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

yacc: *.y
	yacc -d --verbose matc.y
#add --debug to debug it

lex: *.l
	flex matc.l

mrproper: clean
	@rm analyseur

clean: remove_obj_only
	@rm main *.s

run:
	./main

run_test:
	./main "out.s" "simple_test.matc"

run_test_mmcheck:
	valgrind --leak-check=full --show-leak-kinds=all ./main "out.s" "simple_test.matc"
