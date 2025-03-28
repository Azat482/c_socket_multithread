#include "database_mng.h"


int create_var(char* name, char* value){
    FILE *database_file = fopen(DATABASE_FILE_PATH, "a+");
    if(!database_file){
        return -1;
    }
    int wfile = fprintf(database_file, "%s\t{%s}\n", name, value);
    if(wfile < 0){
        fclose(database_file);
        return -2;
    }
    fclose(database_file);
    return 1;
}

int update_var(char* name, char* new_value){
    char var_string[VARIABLE_NAME_SIZE + TEXT_VALUE_SIZE];
    char new_var_string[VARIABLE_NAME_SIZE + TEXT_VALUE_SIZE];
    char var_name[VARIABLE_NAME_SIZE];
    fpos_t file_position;
    FILE *database_file = fopen(DATABASE_FILE_PATH, "r+");
    if(!database_file){
        return -1;
    }
    fgetpos(database_file, &file_position);
    while(fgets(var_string, sizeof(var_string), database_file)){
        sscanf(var_string, "%s", var_name);
        if(!strcmp(var_name, name)){
            int var_string_len = strlen(var_string);
            strcpy(new_var_string, "");
            strcat(new_var_string, var_name);
            strcat(new_var_string, "\t");
            strcat(new_var_string, "{");
            strcat(new_var_string, new_value);
            strcat(new_var_string, "}");
            int i = strlen(var_name) + strlen("\t") + strlen("{") + strlen(new_value) + strlen("}");
            while (i < var_string_len - 1)
            {   
                strcat(new_var_string, "#");
                i++;
            }
            strcat(new_var_string, "\n");

            printf("upd:\t%s\n%d\t%d\n", new_var_string, strlen(var_string), strlen(new_var_string));
            if(strlen(new_var_string) > strlen(var_string)){
                int n_sym = strlen(new_var_string) - strlen(var_string);
                printf("right shift of data on [%d] symbols\n", n_sym);
                long int start_pos, cur_pos, end_pos;
                start_pos = ftell(database_file);
                int n_sym_i;
                for(n_sym_i = 0; n_sym_i < n_sym; n_sym_i++){
                    fseek(database_file, 0, SEEK_END);
                    cur_pos = ftell(database_file);
                    end_pos = ftell(database_file);
                    long int sh_i = 0;

                    int lim = 0;
                    while(cur_pos != start_pos && lim < 100)
                    {      
                        printf("%d\t%d\n", start_pos, cur_pos);
                        fseek(database_file, end_pos, SEEK_SET);
                        fseek(database_file, (sh_i * (-1)), SEEK_CUR);
                        char prev_ch = getc(database_file);
                        printf("getc: %c\n", prev_ch);
                        fputc((int)prev_ch, database_file);
                        sh_i++;
                        cur_pos = ftell(database_file);

                        lim++;
                    }

                }
            }

            fsetpos(database_file, &file_position);
            fputs(new_var_string, database_file);
            fclose(database_file);
            return 1;
        }
        fgetpos(database_file, &file_position); 
    }
    fclose(database_file);
    return 0; 
}


int delete_var(char* name){
    char var_string[VARIABLE_NAME_SIZE + TEXT_VALUE_SIZE];
    char cleaning_var_string[VARIABLE_NAME_SIZE + TEXT_VALUE_SIZE];
    char var_name[VARIABLE_NAME_SIZE];
    fpos_t file_position;
    FILE *database_file = fopen(DATABASE_FILE_PATH, "r+");
    if(!database_file){
        return -1;
    }
    fgetpos(database_file, &file_position);
    while(fgets(var_string, sizeof(var_string), database_file)){
        sscanf(var_string, "%s", var_name);
        if(!strcmp(var_name, name)){
            int var_string_len = strlen(var_string);
            strcpy(cleaning_var_string, "");
            int i = 0;
            while (i < var_string_len - 1)
            {   
                strcat(cleaning_var_string, "#");
                i++;
            }
            strcat(cleaning_var_string, "\n");
            fsetpos(database_file, &file_position);
            fputs(cleaning_var_string, database_file);
            fclose(database_file);
            return 1;
        }
        fgetpos(database_file, &file_position); 
    }
    fclose(database_file);
    return 0;
}


int get_var(char* name, char* value){
    char var_string[VARIABLE_NAME_SIZE + TEXT_VALUE_SIZE];
    char var_name[VARIABLE_NAME_SIZE];
    fpos_t file_position;
    FILE *database_file = fopen(DATABASE_FILE_PATH, "r");
    if(!database_file){
        return -1;
    }
    fgetpos(database_file, &file_position);
    while(fgets(var_string, sizeof(var_string), database_file)){
        sscanf(var_string, "%s", var_name);
        if(!strcmp(var_name, name)){
            int var_string_len = strlen(var_string);
            int i;
            for(i = 0; i < var_string_len; i++){
                if(var_string[i] == '{'){
                    i++;
                    break;
                }
            }
            int value_index;
            for (value_index = 0; var_string[i] != '}' && i < var_string_len && value_index < TEXT_VALUE_SIZE ; value_index++, i++)
            {
                value[value_index] = var_string[i];
            }
            value[value_index] = '\0';
            fclose(database_file);
            return 1;
        }
        fgetpos(database_file, &file_position); 
    }
    fclose(database_file);
    return 0; 
}