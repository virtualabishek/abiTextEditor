#include <ctype.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


struct termios orig_termios;

// to save the setting or config
void disableRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// not print the character
void enableRawMode() {
	tcgetattr(STDIN_FILENO, &orig_termios);
	// to register our disableRawMode() function to be called automatically when the program exits
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	// DISABLE ctrl + s and ctrl + q
	raw.c_lflag &= ~(IXON);

	// ICANON flag that allows us to turn off canonical mode
	// ISIG is for turning off  ctrl + c and ctrl + z
	// IEXTEN for ctrl + v
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main() {
	char c;
	while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		if(iscntrl(c)) {
			printf("%d\n", c);
		}
		else {
			printf("%d ('%c')\n", c,c);
	}
}
	return 0;
}
