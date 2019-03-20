#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILENAME_SIZE 50
extern char *optarg;

int main(int argc, char *const argv[]) {
    int tokenize = 1;
    enum lang {
        ASCII,
        English,
    };
    enum lang ip_lang = English;
    char ip_file[FILENAME_SIZE], op_file[FILENAME_SIZE];
    // Linux specific
    snprintf(ip_file, sizeof(ip_file), "%s", "/dev/stdin");
    snprintf(op_file, sizeof(op_file), "%s", "/dev/stdout");
    const char *optstring = "l:i:o:td";
    int option;
    while ((option = getopt(argc, argv, optstring)) != -1) {
        switch (option) {
            case 'l':
                if (strcmp(optarg, "English") == 0)
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
            case 't':
            case 'd':
                tokenize = (option == 't');
                break;
        }
    }
    FILE *ip_fptr, *op_fptr;
    if ((ip_fptr = fopen(ip_file, "r")) == NULL) {
        printf("Cannot open input file\n");
        return 1;
    }
    op_fptr = fopen(op_file, "w");
    if ((op_fptr = fopen(op_file, "w")) == NULL) {
        printf("Cannot open output file\n");
        return 1;
    }
    if (tokenize) {
        if (ip_lang == ASCII) {
            char ch;
            while (fscanf(ip_fptr, "%c", &ch) != EOF) {
                fprintf(op_fptr, "%c\n", ch);
            }
        } else if (ip_lang == English) {
            const int MAX_TOKEN_LENGTH = 100;
            char token[MAX_TOKEN_LENGTH + 1], ch;
            int i = 0;
            while (fscanf(ip_fptr, "%c", &ch) != EOF) {
                if (isalpha(ch)) {
                    if (i < sizeof(token) - 1) {
                        token[i++] = ch;
                    } else {
                        token[sizeof(token) - 1] = '\0';
                        fprintf(op_fptr, "%s\n", token);
                        token[0] = ch;
                        i = 1;
                    }
                } else if (i > 0) {
                    token[i] = '\0';
                    fprintf(op_fptr, "%s\n%c\n", token, ch);
                    i = 0;
                } else {
                    fprintf(op_fptr, "%c\n", ch);
                }
            }
        }
    } else {
        if (ip_lang == ASCII) {
            char line[3];
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                if (line[0] == '\n')
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                fprintf(op_fptr, "%c", line[0]);
            }
        } else if (ip_lang == English) {
            const int MAX_TOKEN_LENGTH = 100;
            char line[MAX_TOKEN_LENGTH + 1 + 1];
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                line[strlen(line) - 1] = '\0';
                if (strlen(line) == 0)
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                fprintf(op_fptr, "%s", line);
            }
        }
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}