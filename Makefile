ajit: main.c asm.c disjunction.c globals.c parser.c include/asm.h include/parser.h include/globals.h
	gcc main.c asm.c disjunction.c parser.c globals.c -I include -lm -lkeystone -g -o ajit
clean:
	rm ajit
run:
	./ajit
