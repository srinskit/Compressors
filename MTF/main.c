#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define FILENAME_SIZE 50
extern char *optarg;

int main(int argc, char *const argv[]) {
    int encode = 0;
    enum lang {ASCII, English,};
    enum lang ip_lang = ASCII;
    char ip_file[FILENAME_SIZE], op_file[FILENAME_SIZE];
    memset(ip_file, '\0', sizeof(ip_file));
    memset(op_file, '\0', sizeof(op_file));
    const char *optstring = "l:i:o:ed";
    int option;
    while((option = getopt(argc, argv, optstring)) != -1){
        switch(option){
            case 'l':
                if(strcmp(optarg, "English") == 0)
                    ip_lang = English;
                else if (strcmp(optarg, "ASCII") == 0)
                    ip_lang = ASCII;                
                break;
            case 'i':
                snprintf(ip_file, sizeof(ip_file), "%s", optarg);
                break;
            case 'o':
                snprintf(op_file, sizeof(op_file), "%s", optarg);
                break;
            case 'e':
            case 'd':
                encode = (option == 'e');
                break;
        }
    }
    FILE *ip_fptr, *op_fptr;
    if((ip_fptr = fopen(ip_file,"r")) == NULL){
        printf("Cannot open input file\n");
        return 1;
    }
    op_fptr = fopen(op_file,"w");
    if((op_fptr = fopen(op_file,"w")) == NULL){
        printf("Cannot open output file\n");
        return 1;
    }
    if(encode){
        if(ip_lang == ASCII){
            
        }
        else if(ip_lang == English){
            const int MAX_TOKEN_LENGTH = 100, STACK_SIZE = 100;
            int i, found;
            char *stack[STACK_SIZE];
            // token + \n + \0
            char line[MAX_TOKEN_LENGTH + 1 + 1];
            for(i=0; i<STACK_SIZE; ++i)
                stack[i] = NULL;
            while(fgets(line, sizeof(line), ip_fptr) != NULL){
                line[strlen(line)-1] = '\0';
                if(strlen(line) == 0){
                    // When OG token is '\n'
                    // Todo: test when OG token is '\0'
                    if(fgets(line, sizeof(line), ip_fptr) == NULL)
                        break;
                }
                char *prev_item = NULL;
                found = 0;
                for(i=0; i< STACK_SIZE && stack[i]!=NULL;++i){
                    char* tmp = stack[i];
                    stack[i] = prev_item;
                    prev_item = tmp;
                    if(strcmp(line, prev_item) == 0){
                        found = 1;
                        break;
                    }
                }
                if(found){
                    fprintf(op_fptr, "%d\n", i);
                    stack[0] = prev_item;
                }
                else{
                    if(i < STACK_SIZE)
                        stack[i] = prev_item;
                    else
                        free(prev_item);
                    fprintf(op_fptr, "%d\n%s\n", -1, line);
                    stack[0] = (char*)malloc(MAX_TOKEN_LENGTH+1);
                    strcpy(stack[0], line);
                }
            }
            for(i=0; i<STACK_SIZE; ++i)
                free(stack[i]);
        }
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}