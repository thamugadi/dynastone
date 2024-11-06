dynastone: main.c asm.c disjunction.c globals.c parser.c include/asm.h include/parser.h include/globals.h
	gcc main.c asm.c disjunction.c parser.c globals.c -I include -lkeystone -o dynastone 
clean:
	rm dynastone 
run:
	./dynastone
