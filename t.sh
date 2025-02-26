#!/bin/bash
./bin/SysYc -S -o debug.out.s test.sy -O1
riscv64-unknown-linux-gnu-gcc debug.out.s -c -o tmp.o -w
riscv64-unknown-linux-gnu-gcc -static tmp.o lib/libsysy_rv.a
qemu-riscv64 ./a.out
echo $? 