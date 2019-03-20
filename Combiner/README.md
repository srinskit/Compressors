# Combiner
## Compile
`gcc main.c -o comb -lm`
## Run
### Options
1. Language: -l (MTF)
2. Task: -c combine
3. Task: -s seperate
4. I/P file: -i input file path
5. O/P file: -o output file path
### Example
~~~~bash
# codes to binary
./comb -c -l MTF -i code_file -o binary_file

# binary to codes
./code -s -l MTF -i binary_file -o code_file

# text to binary
./tokenizer -t -l English -i str_file | ./mtf -e -l English | ./comb -c -l MTF -o binary_file

# binary to text
./comb -s -l MTF -i binary_file | ./mtf -d -l English | ./tokenizer -d -l English -o str_file

# Verify
./tokenizer -t -l English -i file1 | ./mtf -e -l English | ./comb -c -l MTF | ./comb -s -l MTF | ./mtf -d -l English  | ./tokenizer -d -l English -o file2
diff file1 file2
~~~~

## Language support
### MTF
Takes MTF code file as input. Outputs MTF code file in binary. Output file will contain 64bit blocks. 