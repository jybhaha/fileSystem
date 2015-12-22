
#include<iostream>

using namespace std;

#include"Disk.h"
#include"FileSystem.h"
#include"misc.h"

int main()
{
	FileSystem fileSystem;
	int errCode;
   
	errCode = fileSystem.formatDisk();

	if(errCode == 1)
		cout<<"Filesystem formatting failed"<<endl;
    
    
    byte buffer[] = "implementing File Systems, a test program";
	int file1 = fileSystem.create(sizeof(buffer));
	int fileid1 = fileSystem.open(file1);
    fileSystem.seek(fileid1, 0, SEEK_CUR);   
    fileSystem.write(fileid1, buffer, 20); 
    cout<<"/******************输入第一个文件后显示**********************/\n";
    fileSystem.show();	
	
	byte buffer2[] = "again";
	int file2 = fileSystem.create(sizeof(buffer2));
	int fileid2 = fileSystem.open(file2);
	fileSystem.seek(fileid2,0,SEEK_CUR);
	fileSystem.write(fileid2,buffer2,6);
	cout<<"/******************输入第二个文件后显示**********************/\n";
	fileSystem.show();


	byte buffer3[] = "three";
    int file3 = fileSystem.create(sizeof(buffer2));
	int fileid3 = fileSystem.open(file3);
	fileSystem.seek(fileid3,0,SEEK_CUR);
	fileSystem.write(fileid3,buffer3,6);
	cout<<"/******************输入第三个文件后显示**********************/\n";
	fileSystem.show();
	
	//最后再调用close（）
	fileSystem.close(fileid3);
    fileSystem.close(fileid2);
    fileSystem.close(fileid1);
	return 0;
}
