programs = colnames schedule reader
$(programs): %: %.c
	gcc -o ${*} ${*}.c

installed_programs=$(addprefix ${HOME}/.local/bin/,$(programs))

$(installed_programs): ${HOME}/.local/bin/%: %
	mv ${*} ${HOME}/.local/bin

install: installed_programs
