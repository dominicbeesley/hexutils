#include <stdio.h>
#include <string.h>
#include "hexfile.h"

char buf[256];

void usage(FILE *fp, char *message) {

	if (message) {
		fprintf(fp, "%s\n", message);
	}

	fprintf(fp,
		"bin2hex <address> <in> <out>\n"
		"\n"
		"Creates an intel hex file from the binary data input.\n"
	);
}

int main (int argc, char **argv) 
{
	int i = 1;
	
	if (argc != 4) {
		usage(stderr, "Incorrect number of parameters");
		return -1;
	}
	
	char *p = argv[1];
	
	int addr = strtol(p, NULL, 16);
	
	FILE *inf = fopen(argv[2], "r");
	if (!inf) 
	{
		fprintf(stderr,"Cannot open input file \"%s\"\n", argv[2]);
		usage(stderr, NULL);		
		return -2;
	}
	
	FILE *ouf = fopen(argv[3], "w");
	if (!ouf)
	{
		fprintf(stderr,"Cannot open output file \"%s\"", argv[3]);
		usage(stderr, NULL);
		return -3;
	}
	
	
	printf("Reading binary in \"%s\" and writing to \"%s\", base address %X\n", argv[2], argv[3], addr);
		
	int err = 0;
	int type, len, addrX;
	int tot = 0;
	unsigned int min_addr = addr;
	do {
		len = fread(buf, 1, 32, inf);
		
		if (len > 0) {
			hexfile_writeline(ouf, buf, HEXFILE_TYPE_DATA, addr, len);
		}
		
		addr += len;
		
	} while (!feof(inf));
	
	hexfile_writeline(ouf, buf, HEXFILE_TYPE_EOF, 0, 0);
	
	printf("%d (%X) bytes written %X - %X\n", tot, tot, addr, addr + tot);
	
	fclose(inf);
	fclose(ouf);
	
	return 0;
}
