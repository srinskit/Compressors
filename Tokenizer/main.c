#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define FILENAME_SIZE 50
extern char *optarg;

int main(int argc, char *const argv[]) {
    enum lang {Bytes, English,};
    enum lang ip_lang = Bytes;
    char ip_file[FILENAME_SIZE], op_file[FILENAME_SIZE];
    memset(ip_file, '\0', sizeof(ip_file));
    memset(op_file, '\0', sizeof(op_file));
    const char *optstring = "l:i:o:";
    int option;
    while((option = getopt(argc, argv, optstring)) != -1){
        switch(option){
            case 'l':
                if(strcmp(optarg, "English") == 0)
                    ip_lang = English;
                else if (strcmp(optarg, "Bytes") == 0)
                    ip_lang = Bytes;                
                break;
            case 'i':
                snprintf(ip_file, sizeof(ip_file), "%s", optarg);
                break;
            case 'o':
                snprintf(op_file, sizeof(op_file), "%s", optarg);
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
    fprintf(op_fptr, "%s\n", "Hello World");
    if(ip_lang == Bytes){
        
    }
    else if(ip_lang == English){
        
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}