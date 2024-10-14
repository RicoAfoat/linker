#!bash

riscv64-unknown-linux-gnu-gcc lib.o main.o -o main -B../build -static
