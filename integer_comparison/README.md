Compile command
=====
```
# Generate binary file and run stat collection
gcc -g -O2 test.c -o test -lrt && ./test
# Generate asm file, you can read the assembly instruction.
gcc -g -O2 test.c -S -lrt
```
