// includes
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// data
//
struct termios orig_termios;

// terminal
// For error handeling
void die(const char *s) {
	perror(s);
	exit(1);
}


// to save the setting or config
void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

// not print the character
void enableRawMode() {
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
	// to register our disableRawMode() function to be called automatically when the program exits
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	// DISABLE ctrl + s and ctrl + q
	// ICRNL is for Conrtrol + M, or \n for a new line
	// Some miscellaneous flags, but not required on the modern terminals
	raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	// Turn off the output proecessing
	raw.c_lflag &= ~(OPOST);

	// ICANON flag that allows us to turn off canonical mode
	// ISIG is for turning off  ctrl + c and ctrl + z
	// IEXTEN for ctrl + v
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	//Minimum numbers of bytes i.e 0
	raw.c_cc[VMIN] = 0;
	// Minimum Time is 100 milisecond, cause 1/10 sec
	raw.c_cc[VTIME] = 1;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

// init or main func
int main() {
	while (1) { 
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
			die("read");
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		}
		else {
			printf("%d ('%c')\r\n", c,c);
	}
		if(c == 'q') break;
}
	return 0;
}
