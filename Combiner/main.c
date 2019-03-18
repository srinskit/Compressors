#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FILENAME_SIZE 50
extern char *optarg;

/**
 * @brief Write data into buffer.
 *
 * Note that when function returns 0, the function has to be recalled with new
 * buffer.
 *
 * @param buff The pointer to the buffer to be written to.
 * @param code The data to be written.
 * @param code_len The length of data to be written in bits.
 * @param flush Set to 1 to flush buffer and reset function.
 * @return int -1 if error, 1 if success, 0 if function needs to be recalled
 * with next buffer.
 * @return int 1 if flush didn't modify buffer, 0 if flush modified buffer.
 */
int buff_write(uint64_t *buff, uint64_t code, unsigned code_len, int flush) {
    const unsigned buff_len = 8 * sizeof(*buff);
    static unsigned vacant_len = buff_len;
    static unsigned to_write_len = 0;
    if (code_len > buff_len) return -1;
    if (flush) {
        if (vacant_len == buff_len) return 1;
        *buff <<= vacant_len;
        vacant_len = buff_len;
        to_write_len = 0;
        return 0;
    }
    if (to_write_len != 0) {
        *buff = code - ((code >> to_write_len) << to_write_len);
        ;
        vacant_len -= to_write_len;
        to_write_len = 0;
        return 1;
    }
    if (code_len <= vacant_len) {
        *buff <<= code_len;
        *buff += code;
        if (vacant_len -= code_len)
            return 1;
        else {
            vacant_len = buff_len;
            return 0;
        }
    }
    to_write_len = code_len - vacant_len;
    *buff <<= vacant_len;
    *buff += code >> to_write_len;
    vacant_len = buff_len;
    return 0;
}

/**
 * @brief Read data from buffer.
 *
 * Note that "code" should be read only when function returns 1. When 0 is
 * returned, call the function again with next buffer to complete reading of
 * "code".
 *
 * @param buff The pointer to the buffer to be read from.
 * @param code The pointer to the variable receiving the data.
 * @param code_len The length of data to be read in bits.
 * @param flush Set to 1 to reset function.
 * @return int -1 if error, 1 if success, 0 if function needs to be recalled
 * with next buffer.
 */
int buff_read(uint64_t *buff, uint64_t *code, unsigned code_len, int flush) {
    const unsigned buff_len = 8 * sizeof(*buff);
    static unsigned filled_len = buff_len;
    static unsigned to_read_len = 0;
    if (code_len > buff_len) return -1;
    if (flush) {
        filled_len = buff_len;
        to_read_len = 0;
        return 1;
    }
    if (to_read_len != 0) {
        *code <<= to_read_len;
        *code += *buff >> (buff_len - to_read_len);
        *buff <<= to_read_len;
        filled_len -= to_read_len;
        to_read_len = 0;
        return 1;
    }
    if (code_len <= filled_len) {
        *code = *buff >> (buff_len - code_len);
        *buff <<= code_len;
        filled_len -= code_len;
        return 1;
    }
    to_read_len = code_len - filled_len;
    *code = *buff >> (buff_len - filled_len);
    filled_len = buff_len;
    return 0;
}

int main(int argc, char *const argv[]) {
    int combine = 1;
    enum lang {
        MTF,
    };
    enum lang ip_lang = MTF;
    char ip_file[FILENAME_SIZE], op_file[FILENAME_SIZE];
    // Linux specific
    snprintf(ip_file, sizeof(ip_file), "%s", "/dev/stdin");
    snprintf(op_file, sizeof(op_file), "%s", "/dev/stdout");
    const char *optstring = "l:i:o:cs";
    int option;
    while ((option = getopt(argc, argv, optstring)) != -1) {
        switch (option) {
            case 'l':
                if (strcmp(optarg, "MTF") == 0) ip_lang = MTF;
                break;
            case 'i':
                snprintf(ip_file, sizeof(ip_file), "%s", optarg);
                break;
            case 'o':
                snprintf(op_file, sizeof(op_file), "%s", optarg);
                break;
            case 'c':
            case 's':
                combine = (option == 'c');
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
    if (combine) {
        if (ip_lang == MTF) {
            const int MAX_TOKEN_LENGTH = 100, STACK_SIZE = 100;
            char line[MAX_TOKEN_LENGTH + 1 + 1];
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                line[strlen(line) - 1] = '\0';
                if (strcmp(line, "-1")) {
                } else {
                }
                if (strlen(line) == 0)
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                fprintf(op_fptr, "%s", line);
            }
        }
    } else {
        if (ip_lang == MTF) {
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
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}