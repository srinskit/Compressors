#define __STDC_FORMAT_MACROS
#include <ctype.h>
#include <inttypes.h>
#include <math.h>
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
        if (vacant_len == buff_len) return 0;
        *buff <<= vacant_len;
        int ret = buff_len - vacant_len;
        vacant_len = buff_len;
        to_write_len = 0;
        return ret;
    }
    if (to_write_len != 0) {
        *buff = code - ((code >> to_write_len) << to_write_len);
        vacant_len -= to_write_len;
        to_write_len = 0;
        return 1;
    }
    if (code_len <= vacant_len) {
        *buff <<= code_len;
        *buff += code;
        vacant_len -= code_len;
        return 1;
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
            unsigned code_len = ceilf(log2f(STACK_SIZE + 1));
            unsigned len_len = ceilf(log2f(MAX_TOKEN_LENGTH));
            uint64_t buff = 0, code;
            while (fgets(line, sizeof(line), ip_fptr) != NULL) {
                line[strlen(line) - 1] = '\0';
                if (strcmp(line, "-1") == 0) {
                    if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                    line[strlen(line) - 1] = '\0';
                    unsigned line_len = strlen(line);
                    if (line_len == 0) {
                        if (fgets(line, sizeof(line), ip_fptr) == NULL) break;
                        line_len = strlen(line);
                    }
                    if (buff_write(&buff, STACK_SIZE, code_len, 0) == 0) {
                        fwrite(&buff, sizeof(buff), 1, op_fptr);
                        buff = 0;
                        buff_write(&buff, STACK_SIZE, code_len, 0);
                    }
                    if (buff_write(&buff, line_len, len_len, 0) == 0) {
                        fwrite(&buff, sizeof(buff), 1, op_fptr);
                        buff = 0;
                        buff_write(&buff, line_len, len_len, 0);
                    }
                    for (int i = 0; i < line_len; ++i) {
                        code = (int)line[i];
                        if (buff_write(&buff, code, 8, 0) == 0) {
                            fwrite(&buff, sizeof(buff), 1, op_fptr);
                            buff = 0;
                            buff_write(&buff, code, 8, 0);
                        }
                    }
                } else {
                    code = strtoumax(line, NULL, 10);
                    if (buff_write(&buff, code, code_len, 0) == 0) {
                        fwrite(&buff, sizeof(buff), 1, op_fptr);
                        buff = 0;
                        buff_write(&buff, code, code_len, 0);
                    }
                }
            }
            if (buff_write(&buff, STACK_SIZE, code_len, 0) == 0) {
                fwrite(&buff, sizeof(buff), 1, op_fptr);
                buff = 0;
                buff_write(&buff, STACK_SIZE, code_len, 0);
            }
            int rem;
            if ((rem = buff_write(&buff, 0, 0, 1)) != 0)
                fwrite(&buff, 1, sizeof(buff), op_fptr);
        }
    } else {
        if (ip_lang == MTF) {
            const int MAX_TOKEN_LENGTH = 100, STACK_SIZE = 100;
            char token[MAX_TOKEN_LENGTH + 1];
            unsigned code_len = ceilf(log2f(STACK_SIZE + 1));
            unsigned len_len = ceilf(log2f(MAX_TOKEN_LENGTH));
            uint64_t buff = 0, code;
            if (fread(&buff, 1, sizeof(buff), ip_fptr) == 0) return 1;
            while (1) {
                code = 0;
                if (buff_read(&buff, &code, code_len, 0) == 0) {
                    if (fread(&buff, 1, sizeof(buff), ip_fptr) == 0) break;
                    buff_read(&buff, &code, code_len, 0);
                }
                if (code == STACK_SIZE) {
                    uint64_t token_len = 0;
                    if (buff_read(&buff, &token_len, len_len, 0) == 0) {
                        if (fread(&buff, 1, sizeof(buff), ip_fptr) == 0) break;
                        buff_read(&buff, &token_len, len_len, 0);
                    }
                    if (token_len == 0) break;
                    int i;
                    for (i = 0; i < token_len; ++i) {
                        code = 0;
                        if (buff_read(&buff, &code, 8, 0) == 0) {
                            if (fread(&buff, 1, sizeof(buff), ip_fptr) == 0)
                                break;
                            buff_read(&buff, &code, 8, 0);
                        }
                        token[i] = code;
                    }
                    token[i] = '\0';
                    fprintf(op_fptr, "-1\n%s\n", token);
                } else {
                    fprintf(op_fptr, "%" PRIu64 "\n", code);
                }
            }
            buff_read(NULL, NULL, 0, 1);
        }
    }
    fclose(ip_fptr);
    fclose(op_fptr);
    return 0;
}