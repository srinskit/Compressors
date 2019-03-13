# MTF encoder
## Compile
`gcc main.c -o mtf`
## Run
### Options
1. Language: -l (English | ASCII)
2. Task: -e encode
3. Task: -d decode
4. I/P file: -i input file path
5. O/P file: -o output file path
### Example
~~~~bash
# tokens to codes
./mtf -e -l ASCII -i token_file -o code_file
./mtf -e -l English -i token_file -o code_file

# codes to tokens
./mtf -d -l ASCII -i code_file -o token_file
./mtf -d -l English -i code_file -o token_file

# text to codes
./tokenizer -t -l English -i str_file | ./mtf -e -l English -o code_file

# codes to text
./mtf -d -l English -i code_file | ./tokenizer -d -l English -o str_file

# Verify
./tokenizer -t -l English -i file1 | ./mtf -e -l English | ./mtf -d -l English  | ./tokenizer -d -l English -o file2
diff file1 file2
~~~~

## Language support
### ASCII
*ASCII not supported yet*
### English
Outputs newline seperated codes. If code is -1, the next line will be a ASCII string.