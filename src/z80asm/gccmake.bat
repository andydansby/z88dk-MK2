@echo off
rem $Id: gccmake.bat,v 1.1 2012/11/05 08:15:51 stefano Exp $

echo *******************
echo * Building z80asm *
echo *******************
gcc    -c -o asmdrctv.o asmdrctv.c
gcc    -c -o avltree.o avltree.c
gcc    -c -o class.o class.c
gcc    -c -o codearea.o codearea.c
gcc    -c -o die.o die.c
gcc    -c -o dynstr.o dynstr.c
gcc    -c -o errors.o errors.c
gcc    -c -o except.o except.c
gcc    -c -o exprprsr.o exprprsr.c
gcc    -c -o file.o file.c
gcc    -c -o hist.o hist.c
gcc    -c -o ldinstr.o ldinstr.c
gcc    -c -o memalloc.o memalloc.c
gcc    -c -o modlink.o modlink.c
gcc    -c -o objfile.o objfile.c
gcc    -c -o options.o options.c
gcc    -c -o prsident.o prsident.c
gcc    -c -o prsline.o prsline.c
gcc    -c -o safestr.o safestr.c
gcc    -c -o strlist.o strlist.c
gcc    -c -o strpool.o strpool.c
gcc    -c -o strutil.o strutil.c
gcc    -c -o symbols.o symbols.c
gcc    -c -o z80asm.o z80asm.c
gcc    -c -o z80instr.o z80instr.c
gcc    -c -o z80pass.o z80pass.c
gcc -o z80asm *.o
copy z80asm.exe ..\..\bin
del z80asm.exe
