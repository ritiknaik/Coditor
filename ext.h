#include"defines.h"
#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN struct window win;
EXTERN struct stack *st;


//---------------raw------------------//

void die(const char *s);
void end_raw_mode();
void set_raw_mode();
int read_key();
void check_keypress();
void init_abuf(buffer *p);
void append_buff(buffer *ab, const char *s, int l);
void free_buff(buffer *ab);

//---------------options--------------//

void Refresh_screen();
void move_cursor(int key);
void editor_scroll();
void open_editor();
void save_editor();
char *editor_prompt(char *prompt);
void editor_find();

//--------------terminal-----------------//

void init_editor();
void print_rows();
void print_status_bar(buffer *ab);
void print_mssg_bar(buffer *ab);
void StatusMessage(const char *fmt, ...);
int win_size(int *rows, int *cols);

//----------------rows--------------------//

void insert_new_line();
void delete_char();
void editor_insert_row(int at, char *s, size_t len);
void del_row(int at);
char *rows_to_string(int *buflen);

void row_insert_char(ROW *row, int at, int c);
void append_string_to_row(ROW *row, char *s, size_t len);
void row_delete_char(ROW *row, int at);
void insert_char(int c);
void editor_update_row(ROW *row);
void free_row(ROW *row);
int editor_sxtorx(ROW *row, int sx);
int row_rx_to_sx(ROW *row, int rx);

//----------------syntax-------------------//
 
void update_syntax(ROW *row);
int map_color(int h);
int separator(int c);
void select_syntax();
void editor_undo();

//-----------------stack--------------------//

void init_stack(stack *s);
void push(stack *s, struct window d);
int isFull_stack(stack s) ;
int isEmpty_stack(stack s);
node* pop(stack *s);