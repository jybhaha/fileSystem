#include<iostream>
#include<cstring>

#include"Disk.h"
#include"FileSystem.h"
#define ZERO 0

FileSystem::FileSystem()
{

	disk = new Disk();

	disk->read(&superBlock);

	for(int i = 0; i < ISIZE; i++)
	{
		disk->read(i+1,&inodeBlockTable[i]);
	}

}

int FileSystem::formatDisk()
{

	//初始化superBlock
	superBlock.size = Disk::NUM_BLOCKS;
	superBlock.isize = ISIZE;
	superBlock.freeList = ISIZE + 1;
	disk->write(&superBlock);

	/* Start allocating the free list
	 * We really don't need numBlocksNeeded but then, it would cost us atmost 1 block extra, so we don't care
	 */
	//int numBlocksNeeded = Disk::NUM_BLOCKS/Disk::POINTERS_PER_BLOCK;
	int freeBlockIndex = ISIZE + 1;
	int freeBlockCounter = ISIZE + 1;// + numBlocksNeeded;

	IndirectBlock inode;
	for(int i = 0; i < Disk::POINTERS_PER_BLOCK; i++)
		inode.ptr[i] = 0;

	for(int i= 0; i < ISIZE; i++)
	{
		disk->write(i+1, &inode);
		disk->read(i+1, &inodeBlockTable[i]);
	}

	//初始化所有block
	IndirectBlock freeBlock;
	while(freeBlockIndex<Disk::NUM_BLOCKS){

		for(int i=0; i<Disk::POINTERS_PER_BLOCK ; ++i){
			freeBlock.ptr[i] = ZERO;
			freeBlockCounter++;
		}
		disk->write(freeBlockIndex, &freeBlock);
		freeBlockIndex++;
	}

	//初始化所有file descriptor table
	for(int i=0; i<MAX_FILES; ++i)
		fileDescTable[i] = 0;

	return 0;
}

int FileSystem::inumber(int fd)
{
	if(fileDescTable[fd] != 0)
		return fileDescTable[fd];
	else
		return -1;
}

int FileSystem::open(int inumber)
{

	/* Scan the File Descriptor Table for an empty entry and return the file descriptor */
	for(int i=0; i<MAX_FILES; ++i){
		if(fileDescTable[i] == 0){
			fileDescTable[i] = fileNum;
			seekPointer[i] = 0;

			/* Locate which Inode Block this Inode can be found in */
			int inodeBlockNumber = ((fileNum-1)/4) + 1;//((inumber-1)/4) + 1;
			/* Write into memory */
			disk->read(inodeBlockNumber, &(inodeBlockTable[inodeBlockNumber-1]));
            disk->write(inodeBlockNumber, &(inodeBlockTable[inodeBlockNumber-1]));

			return i;
		}
	}

	/* Failed */
	return -1;
}

int FileSystem::close(int fd)
{
	int inumber = fileDescTable[fd];

	/* Locate which Inode Block this Inode can be found in */
	int inodeBlockNumber = ((inumber-1)/4) + 1;

	/* Write back to disk*/

	disk->write(inodeBlockNumber, &(inodeBlockTable[inodeBlockNumber]));

	fileDescTable[fd] = 0;
	return 0;
}

int FileSystem::seek(int fd, int offset, int whence)
{

	switch(whence){
		case SEEK_SET:
			seekPointer[fd] = offset;
			return seekPointer[fd];
		case SEEK_CUR:
			seekPointer[fd] += offset;
			return seekPointer[fd];
		case SEEK_END:
			//seekPointer[fd] = file_size + offset;
			break;
	}
	return -1;
}

int FileSystem::write(int fd, byte* buffer, int length)
{

	int inumber = fileDescTable[fd];
	int inodeBlockNumber = ((inumber-1)/4);
	Inode inode = inodeBlockTable[inodeBlockNumber].node[(inumber-1)%4];

	int seekPtr = seekPointer[fd];

	if(seekPtr<0)
		return -1;

	int origLength = length;

	int blockNo;
	IndirectBlock indirectBlock;
	char buf_filesys[Disk::BLOCK_SIZE];
	int pos = 0;
	int offset;

	while(length > 0){
		/* Locate the block on the disk */
		if(seekPtr<10*Disk::BLOCK_SIZE){
			blockNo = inode.ptr[seekPtr/Disk::BLOCK_SIZE];
			if(blockNo <= 0){
				blockNo = allocate();
				inode.ptr[seekPtr/Disk::BLOCK_SIZE] = blockNo;
				inodeBlockTable[inodeBlockNumber].node[(inumber -1)%4] = inode;
				disk->write(inodeBlockNumber+1, &inodeBlockTable[inodeBlockNumber]);

			}
		}
		seekPtr -= 10*Disk::BLOCK_SIZE;

		if(seekPtr>0 && seekPtr<64*Disk::BLOCK_SIZE){
			if(inode.ptr[10] == 0){
				inode.ptr[10] = allocate();
			}
			disk->read(inode.ptr[10], &indirectBlock);
			blockNo = indirectBlock.ptr[seekPtr/Disk::BLOCK_SIZE];
			if(blockNo <= 0){
				blockNo = allocate();
				indirectBlock.ptr[seekPtr/Disk::BLOCK_SIZE] = blockNo;
				disk->write(inode.ptr[10], &indirectBlock);
			}

		}
		seekPtr -= 64*Disk::BLOCK_SIZE;

		if(seekPtr>0 && seekPtr<64*64*Disk::BLOCK_SIZE){
			if(inode.ptr[11] == 0){
				inode.ptr[11] = allocate();
			}
			disk->read(inode.ptr[11], &indirectBlock);
			if(indirectBlock.ptr[seekPtr/(64*Disk::BLOCK_SIZE)] == 0){
				indirectBlock.ptr[seekPtr/(64*Disk::BLOCK_SIZE)] = allocate();
				disk->write(inode.ptr[11], &indirectBlock);
			}
			disk->read(indirectBlock.ptr[seekPtr/(64*Disk::BLOCK_SIZE)], &indirectBlock);
			blockNo = indirectBlock.ptr[(seekPtr%(64*Disk::BLOCK_SIZE))/Disk::BLOCK_SIZE];
			if(blockNo <= 0){
				blockNo = allocate();
				indirectBlock.ptr[(seekPtr%(64*Disk::BLOCK_SIZE))/Disk::BLOCK_SIZE] = blockNo;
				disk->write(indirectBlock.ptr[seekPtr/(64*Disk::BLOCK_SIZE)], &indirectBlock);
			}
		}
		seekPtr -= 64*64*Disk::BLOCK_SIZE;

		if(seekPtr>0){
			if(inode.ptr[12] == 0){
				inode.ptr[12] = allocate();
			}
			disk->read(inode.ptr[12], &indirectBlock);
			if(indirectBlock.ptr[(seekPtr/(Disk::BLOCK_SIZE*64*64))] == 0){
				indirectBlock.ptr[(seekPtr/(Disk::BLOCK_SIZE*64*64))] = allocate();
				disk->write(inode.ptr[12], &indirectBlock);
			}
			disk->read(indirectBlock.ptr[(seekPtr/(Disk::BLOCK_SIZE*64*64))], &indirectBlock);
			if(indirectBlock.ptr[(seekPtr%(Disk::BLOCK_SIZE*64*64))/(64*Disk::BLOCK_SIZE)] == 0){
				indirectBlock.ptr[(seekPtr%(Disk::BLOCK_SIZE*64*64))/(64*Disk::BLOCK_SIZE)] = allocate();
				disk->write(indirectBlock.ptr[(seekPtr/(Disk::BLOCK_SIZE*64*64))], &indirectBlock);
			}
			disk->read(indirectBlock.ptr[(seekPtr%(Disk::BLOCK_SIZE*64*64))/(64*Disk::BLOCK_SIZE)], &indirectBlock);
			blockNo = indirectBlock.ptr[(seekPtr%(64*Disk::BLOCK_SIZE))/Disk::BLOCK_SIZE];
			if(blockNo <= 0){
				blockNo = allocate();
				indirectBlock.ptr[(seekPtr%(64*Disk::BLOCK_SIZE))/Disk::BLOCK_SIZE] = blockNo;
				disk->write(indirectBlock.ptr[(seekPtr%(Disk::BLOCK_SIZE*64*64))/(64*Disk::BLOCK_SIZE)], &indirectBlock);
			}
		}

		disk->read(blockNo, buf_filesys);

		offset = seekPointer[fd]%Disk::BLOCK_SIZE;
		if(length<Disk::BLOCK_SIZE){
			memcpy((char *)&buf_filesys[offset], (char *)&buffer[pos], length);
			cout<<buf_filesys<<endl;
			seekPointer[fd] += length;
		}
		else{
			memcpy(&buf_filesys[offset], &buffer[pos], Disk::BLOCK_SIZE - offset);
			seekPointer[fd] += Disk::BLOCK_SIZE - offset;
		}

		cerr<<"Writing \""<<&buf_filesys[offset]<<"\" at block number "<<blockNo<<endl;
		disk->write(blockNo, buf_filesys);

		pos += Disk::BLOCK_SIZE;
		seekPtr = seekPointer[fd];
		length -= Disk::BLOCK_SIZE;
	}

	inodeBlockTable[inodeBlockNumber].node[(inumber-1)%4] = inode;
	disk->write(inodeBlockNumber+1, &inodeBlockTable[inodeBlockNumber]);
	 byte buf_test[256];
	disk->read(blockNo, buf_test);

	return origLength;
}

//找到空白的Block，记录位置
int FileSystem::allocate()
{

	int freeList = superBlock.freeList;
	IndirectBlock freeListBlock;
	disk->read(freeList, &freeListBlock);


	int freeBlock = freeList;
	int counter = 0;
	bool wholeBlockZero=false;


    while(!wholeBlockZero){
			//当该字节不为零，说明本行（即本BLock）有数据，直接寻找下一行
      if(freeListBlock.ptr[counter] != 0){

			freeListBlock.ptr[counter] = 0;
			disk->read(freeList, &freeListBlock);
			freeBlock = ++freeList;
            counter=0;
            continue;
		}

        counter+=1;
				//当counter==64时，说明本行已经遍历完，本行全部是零，BLock为空
		if(counter == Disk::POINTERS_PER_BLOCK){
			freeList++;
			wholeBlockZero=true;
			disk->read(freeList, &freeListBlock);
		}

    }

	char buffer[Disk::BLOCK_SIZE] = {0};
	disk->write(freeBlock, buffer);

    //修改superBlock
    superBlock.size = Disk::NUM_BLOCKS;
	superBlock.isize = ISIZE;
	superBlock.freeList = freeList;
    disk->write(&superBlock);

	return freeBlock;
}

int FileSystem::free(int blockNo)
{
	int freeBlockIndex = (blockNo - (1+ISIZE+Disk::NUM_BLOCKS/Disk::POINTERS_PER_BLOCK))/Disk::POINTERS_PER_BLOCK;
	int freeList = superBlock.freeList;
	IndirectBlock freeListBlock;
	disk->read(freeList+freeBlockIndex, &freeListBlock);

	freeListBlock.ptr[(blockNo - (1+ISIZE+Disk::NUM_BLOCKS/Disk::POINTERS_PER_BLOCK))%Disk::POINTERS_PER_BLOCK] = blockNo;
	disk->write(freeList+freeBlockIndex, &freeListBlock);

	return 0;
}

//创建fileSystem，注意创建文件是要修改filenum
int FileSystem::create(int sizeOfFile)
{
    fileNum++;

	int inode;
	InodeBlock inodeBlock;

	/* Search existing Inode Block Table for free entries */
	for(int i=0; i<ISIZE; ++i){
		inodeBlock = inodeBlockTable[i];
		for(int j=0; j<4; ++j)
			if(inodeBlock.node[j].flags ==0){//修改inodeBLockTable

				inodeBlock.node[j].flags = 1;
				inodeBlock.node[j].owner = 1;
				inodeBlock.node[j].size = sizeOfFile;

				inode = i*4+j+1;

				inodeBlockTable[i] = inodeBlock;
				disk->write(i+1, &inodeBlockTable[i]);

				return inode;
			}


	}




	return -1;


}


void FileSystem::show()
{

	cout<<"--- Dump begins ---"<<endl<<endl;
	IndirectBlock test;

	for(int i = 0; i < 30; i++){
		disk->read(i, &test);
		for(int j = 0; j < Disk::POINTERS_PER_BLOCK; j++)
			cout << test.ptr[j] << " ";
		cout << endl;
	}
	cout<<endl<<"--- Dump ends ---"<<endl;


}
