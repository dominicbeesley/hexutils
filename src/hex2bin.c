#include <stdio.h>
#include <string.h>
#include "hexfile.h"

char buf[256];

void usage(FILE *fp, char *message) {

	if (message) {
		fprintf(fp, "%s\n", message);
	}

	fprintf(fp,
		"hex2bin <start address> <in> <out>\n"
		"\n"
		"Writes data from hex file to binary file starting at given address.\n"
		"Data from before address is ignored\n"
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
	
	
	printf("Dumping from address %X in \"%s\" and writing to \"%s\"\n", addr, argv[2], argv[3]);
		
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
				
		if (type == HEXFILE_TYPE_DATA) {
			int addrNew = addrX - addr;
			p = buf;
			if (addrNew + len > 0) 
			{
				if (addrNew < 0) {
					p -= addrNew;
					len += addrNew;
					addrNew = 0;
				}

				err = fseek(ouf, addrNew, SEEK_SET);
				if (err)
				{
					fprintf(stderr, "Error seeking bin file: %d", err);
					return -4;
				}
				
				err = fwrite(p, 1, len, ouf);			
				if (!err)
				{
					fprintf(stderr, "Error writing bin file: %d", err);
					return -4;
				}

				if (addrNew < min_addr)
					min_addr = addrNew;
				if (addrNew + len > max_addr)
					max_addr = addrNew + len;

				tot += len;
			}

		}
	} while (type != HEXFILE_TYPE_EOF);
	
	fclose(inf);
	fclose(ouf);

	printf("%d (%X) bytes written %X - %X\n", tot, tot, min_addr, max_addr);
	
	return 0;
}
