
all: exe clean run

exe: yacc lex
	gcc -o exe *.c -ly -lfl -lm

yacc: *.y
	yacc -d *.y

lex: *.l
	flex *.l

mrproper: clean
	@rm analyseur

clean:
	@rm lex.* y.*

run:
	./exe 
