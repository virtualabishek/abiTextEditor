// includes
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// data
// Global States

struct editorConfig {
	int screenrows;
	int screencols;
	struct termios orig_termios;
};

struct editorConfig E;
// defines
// for a control key, using hexa, cause it is more easy.
// What the hell haha, define ko paxadi semicolon chaidaina raicha cause this is a macro and 
// This wil be extra macro
#define CTRL_KEY(k) ((k) & 0x1f)


// terminal
// For error handeling
void die(const char *s) {
	// for this next 2 line see below for what this does kata re editorDraw vanda mathi ko comment
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}



// to save the setting or config
void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
		die("tcsetattr");
}



// not print the character
void enableRawMode() {
	if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
		die("tcgetattr");
	// to register our disableRawMode() function to be called automatically when the program exits
	atexit(disableRawMode);
	struct termios raw = E.orig_termios;
	// DISABLE ctrl + s and ctrl + q
	// ICRNL is for Conrtrol + M, or \n for a new line
	// Some miscellaneous flags, but not required on the modern terminals
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	// Turn off the output proecessing
	raw.c_oflag &= ~(OPOST);

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


// function that return
/*** terminal section ***/
char editorReadKey() {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if(nread == -1 && errno != EAGAIN ) die("read");
	}
	return c;
}

// for the detail of the Cursor

int getCursorPosition(int *rows, int *cols) {
	if(write(STDOUT_FILENO, "\x1b[6n", 4) !=4) return -1;
	print("\r\n");
	char c;
	while(read(STDIN_FILENO, &c, 1) ==1) {
		if(iscntrl(c)) {
			print("\r\n", c);
		} else {
			print("%d ('%c')\r\n",c, c);
		}
	}
	editorReadKey();
	return -1;

}


// For widnow size, aba kati pata lagauna paryo
// ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>.
// If sucess ioct1() replace wides, and rows., on fail return 01
int getWindowSize(int *rows, int *cols) {
	struct winsize ws;

	if(1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12)!=12) return -1;
		editorReadKey();
		return getCursorPosition(rows, cols);
	} else {
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

// Output Section
// 4 means, we are writing 4 bytes out to the terminal
// first bytes is \x1b which is escape character or 27 in decimal

// for a tildes sign 

void editorDrawRows() {
	int y;
	for(y =0; y<E.screenrows; y++) {
		write(STDOUT_FILENO, "~\r\n", 3);
	}
}


void editorRefreshScreen() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	editorDrawRows();
	write(STDOUT_FILENO, "\x1b[H", 3);

}

// for input and doesnt return
/*** input  section***/ 
void editorProcessKeyPress() {
	char c = editorReadKey();
	switch (c) {
		case CTRL_KEY('q'):
		  write(STDOUT_FILENO, "\x1b[2J", 4);
		  write(STDOUT_FILENO, "\x1b[H", 3);
		  exit(0);
		  break;
	  }
}

//init
void initEditor(){
	if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

// init or main func
int main() {
	enableRawMode();
	initEditor();
	while (1) { 
		editorRefreshScreen();
		editorProcessKeyPress();
	}
	return 0;
}

// At Step 32
