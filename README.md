# ELFLoader

This is a ELF object in memory loader/runner. The goal is to create a single 
elf loader that can be used to run follow on capabilities across all x86_64 and  
x86 nix operating systems.

## How it works
The way this loader works is that it has all the code needed to build and load
an ELF object for the OS its been compiled for, anything that gets sent to it
that isn't an elf file, isn't for the right arch, or if it links to something 
that can't be resolved it ends up exiting out without attempting to run.

## Resolving Symbols
A key difference between the COFFLoader and this is that it uses the standard
definitions from the OS's libc instead of having to redefine every function used.

## Project Layout

``` sh
.
├── includes
│   ├── beacon_api.h
│   ├── beacon_compatibility.h
│   ├── debug.h
│   ├── ELFLoader.h
│   └── minimal_elf.h
├── Makefile
├── README.md
├── SA
│   └── src
│       ├── cat.c
│       ├── chmod.c
│       ├── env.c
│       ├── find.c
│       ├── grep.c
│       ├── id.c
│       ├── ifconfig.c
│       ├── pwd.c
│       ├── tasklist.c
│       ├── uname.c
│       ├── walk.c
│       └── whoami.c
├── Scripts
│   └── beacon_generate.py
├── src
│   ├── beacon_compatibility.c
│   └── ELFLoader.c
└── testobjects
    ├── getuid.c
    ├── test2.c
    ├── test2_duplicatetext.c
    └── test.c

6 directories, 26 files
```
