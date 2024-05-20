programs=colnames schedule

build:
	mkdir -p build

$(addprefix build/,$(programs)): build/%: %.c | build
	gcc -o build/${*} ${*}.c

installed_programs=$(addprefix ${HOME}/.local/bin/,$(programs))

$(installed_programs): ${HOME}/.local/bin/%: build/%
	cp build/${*} ${HOME}/.local/bin

install: $(installed_programs)
