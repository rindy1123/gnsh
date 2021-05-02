gnsh: clean
	gcc -o gnsh gnsh.c -Werror -Wall

clean:
	rm -f gnsh
