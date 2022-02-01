#include"ext.h"



//--------------------database for file types-------------------//

char *C_HL[] = {".c", ".cpp", ".h", NULL};
char *C_KEY[] = {
    "void|", "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|","#include|","#define|"
    "if", "else", "for", "do","while", "break", "continue", "switch", "return",
    "typedef", "struct", "class", "case", "static", "enum", "union", NULL};

char *PY_HL[] = {".py", NULL};
char *PY_KEY[] = {
    "and|", "or|", "not|", "lambda|", "None|", "is|","class|", "True|", "False|", "return|", "def|", "pass|", "raise|" ,"assert|",
    "if", "else", "elif", "for", "while", "import", "from", "continue", "break","private","protected","public",
    "except", "try", "finally", "in", NULL};

char *JS_HL[] = {".js", ".jsx", NULL};
char *JS_KEY[] = {
    "const|", "let|", "var|", "void|", "static|", "new|", "import|", "export|", "private|", "protected|", "public|",
    "int", "char", "boolean", "abstract", "this", "try", "export", "function", "static", "long",
    "for", "while", "if", "else", "class", "continue", "break", "this", NULL};

char *JAVA_HL[] = {".java", NULL};
char *JAVA_KEY[] = {
    "abstract|", "boolean|", "catch|", "case|", "default|", "double|", "char|", "float|", "int|", "void|", "static|",
    "for", "do", "while", "if", "else", "switch", "break", "continue", "goto", "class", "private", "protected", "public", "return", "this",
    "const", "import", "try", "throw", NULL};

char *TXT_HL[] = {".txt", NULL};
char *TXT_KEY[] = {NULL};

struct syntax DB[] = {
    {"c",
     C_HL,
     "//",
     C_KEY,
     HL_HIGHLIGHT_NUMBERS | Hl_HIGHLIGHT_STRINGS},
     {"python",
     PY_HL,
     "#",
     PY_KEY,
     HL_HIGHLIGHT_NUMBERS | Hl_HIGHLIGHT_STRINGS
     },
     {"javascript",
     JS_HL,
     "//",
     JS_KEY,
     HL_HIGHLIGHT_NUMBERS | Hl_HIGHLIGHT_STRINGS
     },
     {"java",
     JAVA_HL,
     "//",
     JAVA_KEY,
     HL_HIGHLIGHT_NUMBERS | Hl_HIGHLIGHT_STRINGS
     },
     {"text",
     TXT_HL,
     "",
     TXT_KEY,
     0 
     },
};

//-------------------syntax highlighting--------------------//


//updates syntax
void update_syntax(ROW *row) {
    row->h = realloc(row->h, row->rsize);
    memset(row->h, H_NORMAL, row->rsize);

    if(win.syntax == NULL)
        return;

    char *sc = win.syntax->comment;
    int sc_len = sc ? strlen(sc) : 0;
    char **key = win.syntax->keywords;

    int prev_sep = 1;
    int in_string = 0;
    int i = 0;
    while(i < row->size){
        char c = row->render[i];
        unsigned char prev_hl = (i > 0) ? row->h[i - 1] : H_NORMAL;

        if(sc_len&& !in_string){
            if(!strncmp(&row->render[i], sc, sc_len)){
                memset(&row->h[i], H_COMMENT, row->rsize - i);
                break;
            }
        }
        if(win.syntax->flags & Hl_HIGHLIGHT_STRINGS){
            if(in_string){
                row->h[i] = H_STRING;
                if(c=='\\' && i + 1< row->size){
                    row->h[i + 1] = H_STRING;
                    i += 2;
                    continue;
                }
                if(c == in_string)
                    in_string = 0;
                i++;
                prev_sep = 1;
                continue;
            }else {
                if(c =='"' || c=='\''){
                    in_string = c;
                    row->h[i] = H_STRING;
                    i++;
                    continue;
                }
            }
        }
        
        if(win.syntax->flags & HL_HIGHLIGHT_NUMBERS){
            if ((isdigit(c) && (prev_sep || prev_hl == H_NUMBER)) || (c=='.' && prev_hl == H_NUMBER) ) {
                row->h[i] = H_NUMBER;
                prev_sep = 0;
                i++;
                continue;
            }
            if(!isalnum(c) && (c=='<' || c=='>' || c=='&'||c=='*'||c=='%'||c=='-'||c=='+'||c=='/'||c=='=')){
                row->h[i] = H_SPECIAL;
                prev_sep = 0;
                i++;
                continue;
            }
            if(!isalnum(c) && (c=='('||c==')'||c=='{'||c=='}')){
                row->h[i] = H_BRACKET;
                prev_sep = 0;
                i++;
                continue;
            }
            
        }

        if(prev_sep){
            int j;
            for (j = 0; key[j]; j++){
                int klen = strlen(key[j]);
                int key2 = key[j][klen - 1] == '|';
                if(key2)
                    klen--;
                if(!strncmp(&row->render[i], key[j], klen) && separator(row->render[i+klen])){
                    memset(&row->h[i], key2 ? H_KEY2 : H_KEY1, klen);
                    i += klen;
                    break;
                }
            }
            if(key[j] != NULL){
                prev_sep = 0;
                continue;
            }
        }
        prev_sep = separator(c);
        i++;
    }
}

//maps keypress to color
int map_color(int h){
    switch(h){
        case H_STRING:
            return 96;
        case H_NUMBER:
            return 33;
        case H_COMMENT:
            return 95;
        case H_KEY1:
            return 91;
        case H_KEY2:
            return 32;
        case H_SPECIAL:
            return 90;
        case H_BRACKET:
            return 36;
        default:
            return 37;
    }
}

//checks if separated
int separator(int c){
    return (isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL);
}

//selects syntax on file type
void select_syntax(){
    win.syntax = NULL;
    if (win.filename == NULL)
        return;

    char *ext = strrchr(win.filename, '.');
    for (unsigned int j = 0; j < DB_SIZE; j++) {
        struct syntax *s = &DB[j];
        unsigned int i = 0;
        while (s->filematch[i]) {
            int is_ext = (s->filematch[i][0] == '.');
            if ((is_ext && ext && !strcmp(ext, s->filematch[i])) ||
                (!is_ext && strstr(win.filename, s->filematch[i]))) {
                win.syntax = s;

                int i;
                for (i = 0; i < win.numrows; i++){
                    update_syntax(&win.row[i]);
                }
                return;
            }
            i++;
        }
    }
}