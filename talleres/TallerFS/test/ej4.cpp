#include <iostream>
#include <assert.h>

#include "hdd.h"
#include "mbr.h"
#include "pentry.h"
#include "ext2fs.h"

using namespace std;

int main(int argc, char ** argv)
{
	HDD hdd("hdd.raw");

	Ext2FS * fs = new Ext2FS(hdd, 1);

	Ext2FSInode * inodoPath = fs->inode_for_path("/grupos/g1/");

	Ext2FSInode *inodo_de_la_nota = fs->get_file_inode_from_dir_inode(inodoPath,"nota.txt");

	unsigned int dir = fs->get_block_address(inodo_de_la_nota, 10);

	unsigned char *bufer;

	fs->read_block(dir, bufer);

	cout << (void*)&bufer;

	return 0;
}
