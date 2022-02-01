#include"ext.h"




int main(int argc, char *argv[]){
    set_raw_mode();
    init_editor();

    //st = (stack *)malloc(sizeof(stack));
    //init_stack(st);
    if(argc >=2){
        open_editor(argv[1]);
    }
    StatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
    
    while (1){
        Refresh_screen();
        //push(st, win);
        check_keypress();
    }
    
    return 0;
}