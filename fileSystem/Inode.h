#ifndef __INODE_H_
#define __INODE_H_

#define DISK_BLOCK_SIZE 256

class Inode{
	public:
		Inode();

		static const int SIZE = 64;
		int flags;
		int owner;
		int size;

		/* First 10 pointers, from 0 to 9, point to data blocks
		 * Entry 10 is a single indirect
		 * Entry 11 is a double indirect
		 * Entry 12, the last pointer points to a triple indirect block
		 */
		int ptr[13];
};

class InodeBlock{
	public:
		static const int SIZE = DISK_BLOCK_SIZE/Inode::SIZE;
		Inode node[SIZE];
};

class IndirectBlock{
	public:
		static const int SIZE = DISK_BLOCK_SIZE/sizeof(int);
		int ptr[SIZE];
};

#endif /* _INODE_H_ */
