#include <stdio.h>
#include <string.h>

#define DATABASE_FILE_PATH "database.txt"
#define TEXT_VALUE_SIZE 1000
#define VARIABLE_NAME_SIZE 50


int create_var(char* name, char* value);
int update_var(char* name, char* value);
int delete_var(char* name);
int get_var(char* name, char* value);