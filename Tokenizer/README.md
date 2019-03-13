# Tokenizer
## Compile
`gcc main.c -o tokenizer`
## Run
### Options
1. Language: -l (English | ASCII)
2. Task: -t tokenize
3. Task: -d de-tokenize
4. I/P file: -i input file path
5. O/P file: -o output file path
### Example
~~~~bash
# text to tokens
./tokenizer -t -l ASCII -i str_file -o token_file
./tokenizer -t -l English -i str_file -o token_file

# tokens to text
./tokenizer -d -l ASCII -i token_file -o str_file
./tokenizer -d -l English -i token_file -o str_file

# Verify
./tokenizer -t -l English -i file1 | ./tokenizer -d -l English -o file2
diff file1 file2
~~~~

## Language support
### ASCII
Outputs newline seperated ASCII characters
### English
Outputs newline seperated tokens. A token is exactly one of the following
* non alphabet
* string of alphabets