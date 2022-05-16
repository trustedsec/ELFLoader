
all: x86_64 x86 test2_dup test2 test whoami whoami32 tasklist tasklist32 cat cat32 uname uname32 id id32 uname uname32 walk walk32 grep grep32 find find32 chmod chmod32 env env32 pwd pwd32 ifconfig ifconfig32

SOURCES:=./src/beacon_compatibility.c ./src/ELFLoader.c

x86_64:
	gcc -Wall -I ./includes/ -DTESTING_MAIN $(SOURCES) -ldl -o ELFLoader.out

x86_64D:
	gcc -g -DDEBUG -Wall -I ./includes/ -DTESTING_MAIN $(SOURCES) -ldl -o ELFLoaderD.out

x86_64so:
	gcc -shared -fPIC -fvisibility=hidden -DLIBRARY -Wall -I ./includes/ -DTESTING_MAIN $(SOURCES) -ldl -o libELFLoader.so

x86_64runner:
	gcc -I ./includes/ runner.c -o runner.out -L . -lELFLoader

x86_64_bsd:
	gcc -Wall -I ./includes/ -DTESTING_MAIN $(SOURCES) -o ELFLoader_bsd.out

x86_64_bsdD:
	gcc -Wall -I ./includes/ -DDEBUG -DTESTING_MAIN $(SOURCES) -o ELFLoader_bsd.out

win64:
	x86_64-w64-mingw32-gcc -I ./includes/ -DTESTING_MAIN $(SOURCES) -o ELFLoader_win64.exe

win64D:
	x86_64-w64-mingw32-gcc -DDEBUG -I ./includes/ -DTESTING_MAIN  $(SOURCES) -o ELFLoaderD_win64.exe

win32:
	i686-w64-mingw32-gcc -I ./includes/ -DTESTING_MAIN $(SOURCES) -o ELFLoader_win32.exe

win32D:
	i686-w64-mingw32-gcc -DDEBUG -I ./includes/ -DTESTING_MAIN $(SOURCES) -o ELFLoaderD_win32.exe

test2_dup:
	gcc -c -fPIC -I ./includes/ testobjects/test2_duplicatetext.c -o testobjects/test2_duplicatetext.o

test2:
	gcc -c -fPIC -I ./includes/ testobjects/test2.c -o testobjects/test2.o

test:
	gcc -c -fPIC -I ./includes/ testobjects/test.c -o testobjects/test.o

x86:
	gcc -m32 -I ./includes/ -DTESTING_MAIN $(SOURCES) -ldl -o ELFLoader32.out

x86D:
	gcc -m32 -DDEBUG -I ./includes/ -DTESTING_MAIN $(SOURCES) -ldl -o ELFLoader32.out
	
# Example of compiling object files for 32 bit x86 architecture.
test32:
	gcc -m32 -c -fno-stack-protector -fno-pie ./testobjects/test.c -o ./testobjects/test32.o

whoami:
	gcc -c -fPIC -I ./includes/ SA/src/whoami.c -o SA/src/whoami.o

whoami32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/whoami.c -o SA/src/whoami32.o

tasklist:
	gcc -c -fPIC -I ./includes/ SA/src/tasklist.c -o SA/src/tasklist.o

tasklist32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/tasklist.c -o SA/src/tasklist32.o

cat:
	gcc -c -fPIC -I ./includes/ SA/src/cat.c -o SA/src/cat.o

cat32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/cat.c -o SA/src/cat32.o

uname:
	gcc -c -fPIC -I ./includes/ SA/src/uname.c -o SA/src/uname.o

uname32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/uname.c -o SA/src/uname32.o

id:
	gcc -c -fPIC -I ./includes/ SA/src/id.c -o SA/src/id.o

id32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/id.c -o SA/src/id32.o

walk:
	gcc -c -fPIC -I ./includes/ SA/src/walk.c -o SA/src/walk.o

walk32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/walk.c -o SA/src/walk32.o

env:
	gcc -c -fPIC -I ./includes/ SA/src/env.c -o SA/src/env.o

env32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/env.c -o SA/src/env32.o

grep:
	gcc -c -fPIC -I ./includes/ SA/src/grep.c -o SA/src/grep.o

grep32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/grep.c -o SA/src/grep32.o

find:
	gcc -c -fPIC -I ./includes/ SA/src/find.c -o SA/src/find.o

find32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/find.c -o SA/src/find32.o

chmod:
	gcc -c -fPIC -I ./includes/ SA/src/chmod.c -o SA/src/chmod.o

chmod32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/chmod.c -o SA/src/chmod32.o

pwd:
	gcc -c -fPIC -I ./includes/ SA/src/pwd.c -o SA/src/pwd.o

pwd32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/pwd.c -o SA/src/pwd32.o

ifconfig:
	gcc -c -fPIC -I ./includes/ SA/src/ifconfig.c -o SA/src/ifconfig.o

ifconfig32:
	gcc -m32 -c -fno-stack-protector -fno-pie -I ./includes/ SA/src/ifconfig.c -o SA/src/ifconfig32.o

clean:
	rm -f ELFLoader*.out
	rm -f ELFLoader*.exe
	rm -f ./SA/src/*.o
	rm -f testobjects/*.o
	rm -f ELFLoader_public
	rm -f libELFLoader.so
.PHONY: x86_64 x86_64D test2_dup test2 test clean
