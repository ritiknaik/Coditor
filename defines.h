#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include<termios.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<stdarg.h>
//#include<ncurses.h>
#include<time.h>
#include<string.h>

#define CTRL_KEY(k) ((k) & 0x1f)


#define ARROW_LEFT 1000
#define ARROW_RIGHT 1001
#define ARROW_UP 1002
#define ARROW_DOWN 1003
#define HOME_KEY 1004
#define END_KEY 1005
#define PAGE_UP 1006
#define PAGE_DOWN 1007
#define DELETE_KEY 1008
#define BACKSPACE 127
#define TAB 4
#define QUIT 1

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define Hl_HIGHLIGHT_STRINGS (1<<1)
#define DB_SIZE (sizeof(DB)/sizeof(DB[0]))


enum Highlight
{
  H_NORMAL = 0,
  H_KEY1,
  H_KEY2,
  H_COMMENT,
  H_STRING,
  H_NUMBER,
  H_SPECIAL,
  H_BRACKET
};

typedef struct buffer {
  char *b;
  int len;
}buffer;


typedef struct ROW{
    int size;
    char *line;
    int rsize;
    char *render;
    unsigned char *h;
}ROW;


struct syntax {
    char *filetype;
    char **filematch;
    char *comment;
    char **keywords;
    int flags;
};

struct window{
    int sx, sy;
    int rx;
    int row_offset;
    int col_offset;
    int scrows;
    int sccols;
    int numrows;
    ROW *row;
    char *filename;
    int changed;
    char statusmsg[80];
    time_t status_time;
    struct syntax *syntax;
    struct termios orig_termios;
};


typedef struct node {
    struct window w;
    struct node *left;
    struct node *right;
} node;

typedef struct stack{
    node *head;
    node *top;
    int len;
}stack;

