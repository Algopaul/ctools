colnames: colnames.c
	gcc -o colnames colnames.c

install: colnames
	mv colnames ~/.local/bin
