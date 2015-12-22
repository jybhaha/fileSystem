CPP=g++
CPPFLAGS= -Wall -g
OBJS= Disk.o FileSystem.o Inode.o
FILES= DISK

main: main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS) main.cpp Disk.o FileSystem.o Inode.o -o main

Disk.o: Disk.cpp
	$(CPP) $(CPPFLAGS) -c Disk.cpp -o Disk.o

FileSystem.o: FileSystem.cpp
	$(CPP) $(CPPFLAGS) -c FileSystem.cpp -o FileSystem.o

Inode.o: Inode.cpp
	$(CPP) $(CPPFLAGS) -c Inode.cpp -o Inode.o

clean:
	rm -rf $(OBJS) 
	rm -rf main
	rm -rf $(FILES)
	rm -rf *~
