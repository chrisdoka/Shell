all: shell.o main.o parser.o command_execution.o
	gcc shell.o main.o parser.o command_execution.o -o shell

main.o: main.c shell.h
	gcc -c main.c  -o main.o

shell.o: shell.c shell.h
	gcc -c shell.c -o shell.o

parser.o: parser.c parser.h shell.h 
	gcc -c parser.c -o parser.o

command_execution.o: command_execution.c parser.h
	gcc -c command_execution.c -o command_execution.o

clean:
	rm -f *.o shell