#include"Disk.h"
#include"misc.h"

#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

class  FileSystem{
	public:
		/* Constructor */
		FileSystem();

		int formatDisk();
		int create(int sizeOfFile);
		int inumber(int fd);
		int open(int inumber);
		int write(int fd, byte* buffer, int length);
		int seek(int fd, int offset, int whence);
		int close(int fd);
		
        void show();

	private:
		int allocate();
		int free(int blockNo);

		Disk* disk;
		static const int ISIZE = 4;
		static const int MAX_FILES = 16;

		SuperBlock superBlock;
		InodeBlock inodeBlockTable[ISIZE];
		int fileDescTable[MAX_FILES];
		int seekPointer[MAX_FILES];
		int fileNum=0;//记录file数量
};

#endif /* _FILE_SYSTEM_H_ */
