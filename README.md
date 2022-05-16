# ELFLoader

This is a ELF object in memory loader/runner. The goal is to create a single 
elf loader that can be used to run follow on capabilities across all x86_64 and x86 nix operating systems.

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

## Examples 

### Example of find.o
Generate to cat the files found, replacement for `find /etc/ -iname *release -exec cat {} \;`
or `cat /etc/*release*`

```
Beacon>reset
Beacon>addString /etc
Beacon>addString release
Beacon>addString
Beacon>addint 1
Beacon>addint 1
Beacon>addint 1
Beacon>generate
b'26000000050000002f657463000800000072656c65617365000100000000010000000100000001000000'
```

```
./ELFLoader.out ./SA/src/find.o 26000000050000002f657463000800000072656c65617365000100000000010000000100000001000000
```

### Example of grep.o
Generate to list all files with rwx memory permissions in /proc.

```
Beacon>reset
Beacon>addString /proc
Beacon>addString rwx
Beacon>addString maps
Beacon>addint 1
Beacon>addint 0
Beacon>addint 0
Beacon>generate
b'27000000060000002f70726f63000400000072777800050000006d61707300010000000000000000000000'
```

```
./ELFLoader.out ./SA/src/grep.o 27000000060000002f70726f63000400000072777800050000006d61707300010000000000000000000000
```

### Example of chmod
Generate the arguments for `chmod 777 /tmp/test.sh`.

```
Beacon>reset
Beacon>addString 777
Beacon>addString /tmp/test.sh
Beacon>generate
b'1900000004000000373737000d0000002f746d702f746573742e736800'
```

```
./ELFLoader.out ./SA/src/chmod.o 1900000004000000373737000d0000002f746d702f746573742e736800
```

### Example of uname
This is the generic example that doesn't take any arguments.

```
./ELFLoader.out SA/src/uname.o
```

### Runner and Library Example

```
make x86_64so
make x86_64runner
LD_LIBRARY_PATH=. ./runner.out ./SA/src/env.o
```
