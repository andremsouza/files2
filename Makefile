cc = gcc
src = main.c reg_delim.c
bin = trab
flags = -Wall

all:
	$(cc) -o $(bin) $(src) $(flags)
	./$(bin)