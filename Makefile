cc = gcc
src = main.c reg_delim.c
bin = trab
flags = -Wall

all:
	$(cc) -o $(bin) $(src) $(flags)
	./$(bin)

clear:
	rm trab 1* 2* 3*
