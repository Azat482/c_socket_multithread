#include <stdio.h>
#include "database_mng.h"


int main(){
    create_var("name", "text");
    create_var("name2", "text123 4567 89");
    create_var("name3", "text3");
    create_var("name4", "tex");

    update_var("name3", "123456789");

    delete_var("name3");
    update_var("name2", "nmegq");

    char text_val[TEXT_VALUE_SIZE];
    get_var("name2", text_val);
    printf("finded value [%s]:\t%s\n", "name" ,text_val);

    return 0;

}