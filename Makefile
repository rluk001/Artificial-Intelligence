compiler=g++

flags=-Wall -Werror -ansi -pedantic

all: bin\
     hw

bin:
	[ ! -d bin ]
	mkdir -p bin;

hw: grid.cpp 
	
	$(compiler) $(flags) grid.cpp -o bin/hw

