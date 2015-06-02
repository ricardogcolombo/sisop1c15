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

	unsigned char bufer[18];
	
	fd_t nota = fs->open("/grupos/g10/nota.txt","r");
	fs->seek(nota,13999);
	fs->read(nota,bufer,17);
	bufer[17]='\0';
	printf("%s\n", bufer);
	fs->close(nota);

	return 0;
}
