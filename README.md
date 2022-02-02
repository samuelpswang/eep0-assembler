# EEP0 Assembler

[![Run on Repl.it](https://repl.it/badge/github/samuelpswang/eep0-assembler)](https://repl.it/github/samuelpswang/eep0-assembler)

**TRY IT: 1) Click Replit badge; 2) Click RUN button in Replit.**

## Intro
Assembler for 16-bit EEP0 CPU designed by ([Professer Clarke at Imperial EEE Department](https://www.imperial.ac.uk/people/t.clarke)).

## Command Line Interface
### Direct Instructions
Type in your assembly mnemonic directly and get the machine code (in binary format). 
'R' in registers have to be capitalized. Numbers can be in decimal format (ex. #20) or in hexadecimal format (ex. 0x14), \'#\' and \0x are necessary. 

Note that program will ignore missing comma. See below for example.

```
> ADD R3, #20
0011110000010100

> ADD R3 #20
0011110000010100

> ADD R3, 0x14
0011110000010100

> JMP R4 #23
Input Error: input is not valid assembly code
```

### Flags

Add the following flags after assembly instructions to modify output. Flags can be used in combined fasion.

* `-x`: show don't care as 'X' instead of '0'
* `-d`: show divider between sections of machine code
* `-h`: show output in hexadecimal instead of binary (will ignore `x` and `d` flags)

Note that program will ignore all other flags (without returning an error). See below for examples.

```
> ADD R3, #20 -x
001111XX00010100

> ADD R3, #20 -d
001|1|11|00|00010100

> ADD R3, #20 -xd
001|1|11|XX|00010100

> ADD R3, #20 -h
0x3C14

> ADD R3, #20 -a
0011110000010100
```

### Input File
Type `input` for program to read your assembly code from `input.txt`, the machine code will be in `output.txt`. Remember to create an `input.txt` beforehand, otherwise you will get `File Not Found Error`. See below for example.
```
> input
Done: output.txt written
```

### Input File with Flags
Flags can be used to modify reading and writing of input files. If used `-h`, `-x` and `-d` will be ignored. If non-sense flags are used, program will print `Input Error: invalid arguments after "input" detected`. See below for examples.
```
> input -h
Done: output.txt written

> input -a
Input Error: invalid arguments after "input" detected
```

## Generate All Possible Commands (Verification)
1. Comment all code under `// Instructions` & `// Command Line` comments.
2. Uncommment all code under `// For Verification Purposes Only`.
3. Compile & run `./main`. 

## Release
* `Version: 1.0.0`
* `Date: 2022/02/01`
