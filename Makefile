colnames: colnames.c
	gcc -o colnames colnames.c

testopts: testopts.c
	gcc -o testopts testopts.c

install: colnames
	cp colnames ~/.local/bin
