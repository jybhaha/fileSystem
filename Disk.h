#include<fstream>

#ifndef _DISK_H_
#define _DISK_H_

#include"Inode.h"
#include"SuperBlock.h"
#include"misc.h"

using namespace std;

class Disk{
	public:
		/* Constructor */
		Disk();

		/* Read */
		void read(int blockNo, byte* buffer);
		void read(SuperBlock* block);
		void read(int blockNo, InodeBlock* block);
		void read(int blockNo, IndirectBlock* block);

		/* Write */
		void write(int blockNo, byte* buffer);
		void write(SuperBlock* block);
		void write(int blockNo, InodeBlock* block);
		void write(int blockNo, IndirectBlock* block);

		/* Stop */
		void stop(bool removeFile);
		void test();	

	private:
		static const int BLOCK_SIZE =	256;
		static const int NUM_BLOCKS =	1024;
		static const int POINTERS_PER_BLOCK = BLOCK_SIZE/sizeof(int);

		fstream* disk_int;
		byte buf_int[BLOCK_SIZE];
		
		friend class FileSystem;
};

#endif	/* _DISK_H_ */
