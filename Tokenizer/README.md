# Tokenizer
## Compile
`gcc main.c -o tokenizer`
## Run
### Options
1. Language: -l (English | ASCII)
2. I/P file: -i input file path
3. O/P file: -o output file path

### Example
`./tokenizer -l English -i test -o /dev/stdout`

`./tokenizer -l ASCII -i test -o /dev/stdout`

## Language support
### ASCII
Outputs newline seperated ASCII characters
### English
Outputs newline seperated tokens. A token is exactly one of the following
* non alphabet
* string of alphabets