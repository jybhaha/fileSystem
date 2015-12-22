# fileSystem

模拟实现linux文件系统。inode、superBlock......

The simulation of the linux file system.

实现环境：

- Ubuntu 14.04

- C++高级操作语言

- vim编辑器

通过**C++**调用linux系统函数，一块硬盘（创建的一个文件，本程序中是DISK）上实现一个简单的linux文件系统。


##主要流程
-	创建fileSystem
-	格式化硬盘（fileSystem::formatDisk）:初始化全部Block，初始化superblock。
-	创建文件（fileSystem::create）：初始化inodeBlockTable。
-	打开文件（fileSystem::open（））: 记录已新建文件数
-	写入字符：（fileSystem::write）:寻找空白block（fileSystem::allocate函数），写入。
-	展示：（fileSystem::show()）,通过屏幕打印出来。

##主要函数
### **fileSystem.cpp**
-	FormatDisk() 当处于没有文件存在状态时初始化文件系统:填写superblock，并初始化所用的block中数据为零。
-	 Shutdown() 关闭文件并关闭Disk。
-	Create() 创建文件。在inodeblock table（即本程序中预设的在superBlocks下的四个inodeBlock）中写入数据。
-	Open() 打开一个已存在的文件。
-	Write() 参数有file descriptor，buffer数组，buffer长度。
-	Seek() 参数有file descriptor，查找写入数据的位置，（这里保留了UNIX文件系统的三个变量：SEEK SET, SEEK CUR, and SEEK END）
-	Close() 参数有file descriptor。关闭文件，File descriptor table和Seek pointer table从移除入口。
-	Close() 参数有file descriptor，把inode写回disk，移除所有入口。
-	Allocate()函数 比较关键，寻找freeblocks的位置，以向后来空白block写入数据。
-	Free()  free一个block，并添加到free list

### **Disk.cpp**
-	Constructor Disk()   作用是建立DISK，如果不存在就初始化superblock为0
-	read() 硬盘读操作。只能读数据的一个block。重载了三次，分别被用来读superblock、inodeblocks、indrection blocks。其中inodeBlocks是一个单位Block，包括4个inode。Inderction blocks 中含有指针指向inodeblocks的指针。
-	Write() 硬盘写操作。只能写数据的一个block, 重载了三次，分别被用来写superblock、inodeblocks、indrection blocks。
-	Stop() 停止硬盘运行。
### **inode.h**
- inode 定义了一个inode。主要信息有flag、owner等信息。
- inodeBlock 。主要读写单位
- inderectBlock 指向inode

。。。

##tips

- 本程序中一个Block含有4个inode，一个inode含有16个int，即64个byte。
- 本程序在一个新建的DISK文件上操作。DISK即硬盘。DISK上共有1024个Block.

##说明

![](https://raw.githubusercontent.com/jybhaha/fileSystem/master/photes/new3-1.png)

1. 演示成果中每一行代表一个block。第一行是**superBlock**.第2~5行为inodeBlockTable，用来存放数据块信息。第6行向后是可以存放数据的Block。
2. 如图所示：第一行中第一个inode（每个inode占用16个字节）中存放的三个数据分别为：
 - 1024：总BLock数
 - 4:	 inodeBlockTable所占大小
 - 6:	freeBlock（即）起始位置
3. 当新建file1，并向其中输入：“任意字符****”。fileSystem通过寻找发现第5个BLock为空，后写入字符串。修改superblock（刚开始时superBlock三个字符分别为1024、4、5），修改inodeBlockTable：
  - 1：flag,当前inode是否已被写入
  - 1：owner
  - 42：文件大小（即输入的字符串大小）
  - 5：存放文件的block位置
4.  第五个Block为存放file1的位置。

**接着新建文件file2**

显示结果如图：
![](https://raw.githubusercontent.com/jybhaha/fileSystem/master/photes/new3-2.png)


**接着新建文件file3**

显示结果如图：

![](https://raw.githubusercontent.com/jybhaha/fileSystem/master/photes/new3-3.png)


**注意**第三个图中inodeBlockTable中（即2~5行），前三个inode的数据（第二行）已经改变。


