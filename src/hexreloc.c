#include <stdio.h>
#include <string.h>
#include "hexfile.h"

char buf[256];

void usage(FILE *fp, char *message) {

	if (message) {
		fprintf(fp, "%s\n", message);
	}

	fprintf(fp,
		"hexreloc [-]<address> <in> <out>\n"
		"\n"
		"Relocates the data in the intel hex file by adding (or subtracting) the given\n"
		"address from all addresses in the data file\n"
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
	int minus = 0;
	if (*p == '-') 
	{
		minus = 1;
		p++;
	}
	
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
	
	
	printf("%sing %X from addresses in \"%s\" and writing to \"%s\"\n", (minus?"subtract":"add"), addr, argv[2], argv[3]);
	
	if (minus)
		addr = -addr;
	
	int err = 0;
	int type, len, addrX;
	int tot = 0;
	unsigned int min_addr = 0xFFFFFFFF;
	unsigned int max_addr = 0x0;
	do {
		err = hexfile_readline(inf, buf, &type, &addrX, &len);
		if (err)
		{
			fprintf(stderr, "Error reading hex file: %d", err);
			return -4;
		}
		
		int addrNew = addrX;
		
		if (type == HEXFILE_TYPE_DATA) {
			addrNew = addrX + addr;
			if (addrNew < min_addr)
				min_addr = addrNew;
			if (addrNew + len > max_addr)
				max_addr = addrNew + len;
		}
		if (addrX < 0) 
		{
			fprintf(stderr, "Address less than 0 encountered original address %x\n", addrX);	
		}
		else
		{
			err = hexfile_writeline(ouf, buf, type, addrNew, len);
			if (err)
			{
				fprintf(stderr, "Error writing hex file: %d", err);
				return -4;
			}
			tot += len;
		}
	} while (type != HEXFILE_TYPE_EOF);
	
	fclose(inf);
	fclose(ouf);
	
	printf("%d (%X) bytes written %X - %X\n", tot, tot, min_addr, max_addr);
	
	return 0;
}
