#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILENAME_SIZE 50
extern char *optarg;

int strlenn(char *s) {
    int i = 0;
    while (*(s++) != '\n') ++i;
    return i;
}

int strdiffn(char *s1, char *s2) {
    int l1 = strlenn(s1), l2 = strlenn(s2);
    if (l1 != l2) return 1;
    for (int i = 0; i < l1; ++i)
        if (s1[i] != s2[i]) return 1;
    return 0;
}

void strcpyn(char *dst, char *src) {
    while (*src != '\n') {
        *dst = *src;
        ++src;
        ++dst;
    }
    *dst = '\n';
}

int main(int argc, char *const argv[]) {
    int encode = 1;
    enum lang {
        ASCII,
        English,
    };
    enum lang ip_lang = English;
    char ip_file[FILENAME_SIZE], op_file[FILENAME_SIZE];
    // Linux specific
    snprintf(ip_file, sizeof(ip_file), "%s", "/dev/stdin");
    snprintf(op_file, sizeof(op_file), "%s", "/dev/stdout");
    const char *optstring = "l:i:o:ed";
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
            case 'e':
            case 'd':
                encode = (option == 'e');
                break;
        }
    }
    FILE *ip_fptr, *op_fptr;
    if ((ip_fptr = fopen(ip_file, "rb")) == NULL) {
        printf("Cannot open input file\n");
        return 1;
    }
    op_fptr = fopen(op_file, "w");
    if ((op_fptr = fopen(op_file, "wb")) == NULL) {
        printf("Cannot open output file\n");
        return 1;
    }
    if (encode) {
        if (ip_lang == ASCII) {
        } else if (ip_lang == English) {
            const int MAX_TOKEN_LENGTH = 15, STACK_SIZE = 255;
            int i, found;
            char *stack[STACK_SIZE];
            // token + \n + \0
            char line[MAX_TOKEN_LENGTH + 1 + 1];
            for (i = 0; i < STACK_SIZE; ++i) stack[i] = NULL;
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                if (strlenn(line) == 0) {
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                }
                char *prev_item = NULL;
                found = 0;
                for (i = 0; i < STACK_SIZE && stack[i] != NULL; ++i) {
                    char *tmp = stack[i];
                    stack[i] = prev_item;
                    prev_item = tmp;
                    if (strdiffn(line, prev_item) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    fprintf(op_fptr, "%d\n", i);
                    stack[0] = prev_item;
                } else {
                    if (i < STACK_SIZE)
                        stack[i] = prev_item;
                    else
                        free(prev_item);
                    fprintf(op_fptr, "-1\n");
                    if (line[0] == '\n') fwrite(line, 1, 1, op_fptr);
                    fwrite(line, 1, strlenn(line) + 1, op_fptr);
                    stack[0] = (char *)malloc(MAX_TOKEN_LENGTH + 1);
                    strcpyn(stack[0], line);
                }
            }
            for (i = 0; i < STACK_SIZE; ++i) free(stack[i]);
        }
    } else {
        if (ip_lang == ASCII) {
        } else if (ip_lang == English) {
            const int MAX_TOKEN_LENGTH = 15, STACK_SIZE = 255;
            int i, found;
            char *stack[STACK_SIZE];
            // token + \n + \0
            char line[MAX_TOKEN_LENGTH + 1 + 1];
            for (i = 0; i < STACK_SIZE; ++i) stack[i] = NULL;
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                line[strlen(line) - 1] = '\0';
                if (strcmp(line, "-1") == 0) {
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                    if (strlenn(line) == 0) {
                        if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                    }
                    char *prev_item = NULL;
                    for (i = 0; i < STACK_SIZE && stack[i] != NULL; ++i) {
                        char *tmp = stack[i];
                        stack[i] = prev_item;
                        prev_item = tmp;
                    }
                    if (i < STACK_SIZE)
                        stack[i] = prev_item;
                    else
                        free(prev_item);
                    stack[0] = (char *)malloc(MAX_TOKEN_LENGTH + 1);
                    strcpyn(stack[0], line);
                    if (line[0] == '\n') fwrite(line, 1, 1, op_fptr);
                    fwrite(line, 1, strlenn(line) + 1, op_fptr);
                } else {
                    char *end;
                    long n = strtol(line, &end, 10);
                    char *prev_item = NULL;
                    for (i = 0; i <= n; ++i) {
                        char *tmp = stack[i];
                        stack[i] = prev_item;
                        prev_item = tmp;
                    }
                    stack[0] = prev_item;
                    if (prev_item[0] == '\n') fwrite(prev_item, 1, 1, op_fptr);
                    fwrite(prev_item, 1, strlenn(prev_item) + 1, op_fptr);
                }
            }
            for (i = 0; i < STACK_SIZE; ++i) free(stack[i]);
        }
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}