gnsh: clean
	gcc -o gnsh gnsh.c -Werror -Wall

clean:
	rm gnsh
