#include"ext.h"


//intialize the editor
void init_editor(){
    win.sx = 0;
    win.sy = 0;
    win.rx = 0;
    win.row_offset = 0;
    win.col_offset = 0;
    win.numrows = 0;
    win.row = NULL;
    win.changed = 0;
    win.filename = NULL;
    win.statusmsg[0] = '\0';
    win.status_time = 0;
    win.syntax = NULL;
    if(win_size(&win.scrows, &win.sccols) == -1){
        die("win_size");
    }
    win.scrows -= 2;
}

//draws rows to the terminal
void print_rows(buffer *ab){
    int y;
    for (y = 0; y < win.scrows; y++) {
      int file_row = y + win.row_offset;
      if (file_row >= win.numrows){
        if (win.numrows == 0 && y == win.scrows / 3 - 2){
            char mssg[30];
            int mssg_len = snprintf(mssg, sizeof(mssg), "Text Editorr --verion 0.0.1");
            if (mssg_len > win.sccols)
              mssg_len = win.sccols;
            int padding = (win.sccols - mssg_len) / 2;
            if (padding){
              append_buff(ab, ">", 1);
              padding--;
            }
            while (padding--)
                append_buff(ab, " ", 1);
            append_buff(ab, mssg, mssg_len);
          }
          else{
            append_buff(ab, ">", 1);
          }
    }
    else{
        int len = win.row[file_row].rsize - win.col_offset;
        if(len < 0)
            len = 0;
        if(len>win.sccols)
            len = win.sccols;
        char *c = &win.row[file_row].render[win.col_offset];
        unsigned char *hl = &win.row[file_row].h[win.col_offset];
        int current_color = -1;
        int j;
        for (j = 0; j < len; j++){
            if(hl[j] == H_NORMAL){
                if(current_color !=-1){
                    append_buff(ab, "\x1b[39m", 5);
                    current_color = -1;
                }
                append_buff(ab, &c[j], 1);
            }
            else{
                int color = map_color(hl[j]);
                if(color != current_color){
                    char buf[16];
                    int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
                    append_buff(ab, buf, clen);
                    current_color = color;
                }
                append_buff(ab, &c[j], 1);
            }
        }
        append_buff(ab, "\x1b[39m", 5);
    }

    append_buff(ab, "\x1b[K", 3);
    append_buff(ab, "\r\n", 2);
  }
}


//draws status bar o the terminal
void print_status_bar(buffer *ab){
    append_buff(ab, "\x1b[41m", 5);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s %s", win.filename ? win.filename : "[Untitled]", win.changed ? "[modified]" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d,%d        ", win.syntax ? win.syntax->filetype : "no filetype", win.sy + 1, win.sx + 1);


    if(len>win.sccols)
        len = win.sccols;
    append_buff(ab, status, len);
    while(len < win.sccols){
        if(win.sccols-len == rlen){
            append_buff(ab, rstatus, rlen);
            break;
        }else{
            append_buff(ab, " ", 1);
            len++;
        }
    }
    append_buff(ab, "\x1b[m", 3);
    append_buff(ab, "\r\n", 2);
    return;
}

//prints status message
void StatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(win.statusmsg, sizeof(win.statusmsg), fmt, ap);
    va_end(ap);
    win.status_time = time(NULL);
}

//draws mssg bar to the terminal
void print_mssg_bar(buffer *ab) {
    append_buff(ab, "\x1b[K", 3);
    int msglen = strlen(win.statusmsg);
    if (msglen > win.sccols) msglen = win.sccols;
    if (msglen && time(NULL) - win.status_time < 5)
        append_buff(ab, win.statusmsg, msglen);
    return;
}

//gets size of current terminal open
int win_size(int *rows, int *cols){
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
