#include <stdio.h>
#include "hexfile.h"
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#define HEXFILE_GETHEX(f, h) 				\
{	temp_c[2] = 0; 					\
	c = fgetc(f);					\
	if (c == EOF) 					\
		return EHEXFILE_EOF;			\
	if (!isxdigit(c)) 				\
		return EHEXFILE_BADLINE;		\
	temp_c[0] = c;					\
	c = fgetc(f);					\
	if (c == EOF) 					\
		return EHEXFILE_EOF;			\
	if (!isxdigit(c)) 				\
		return EHEXFILE_BADLINE;		\
	temp_c[1] = c;					\
	h = strtol(temp_c, NULL, 16);			\
	if (errno == EINVAL)				\
		return EHEXFILE_BADLINE;		\
	checksum += h;					\
}
	

int hexfile_readline(
	FILE *fp
,	void *buf
,	unsigned int *type
,	unsigned int *addr
,	unsigned int *len) {
	
	unsigned char *buf2 = (unsigned char *)buf;

	/* synchronise to start of line */
	
	int c, i;
	unsigned int checksum;
	char temp_c[3];
	
	checksum = 0;
	
	do {
		c = fgetc(fp);
		if (c == EOF) {
			return EHEXFILE_EOF;
		} else if (c != ':' && c != '\n' && c != '\r') {
			return EHEXFILE_BADLINE;
		}
	} while ( c != ':' );
	
	
	/* get line length */	
	HEXFILE_GETHEX(fp, *len);
		
	
	/* get address */
	HEXFILE_GETHEX(fp, c);
	*addr = c << 8;
	HEXFILE_GETHEX(fp, c);
	*addr |= c;
	
	/* get type */
	HEXFILE_GETHEX(fp, *type);
	
	/* get data */
	for (i=0; i<*len; i++) {
		HEXFILE_GETHEX(fp, buf2[i]);
	}
	
	/* get checksum */
	HEXFILE_GETHEX(fp, c);
	
	if ((checksum & 0xFF) != 0)
		return EHEXFILE_CKSUM;
	
		
	do {
		c = fgetc(fp);
		if (c == EOF) {
			if (*type == HEXFILE_TYPE_EOF)
				return EHEXFILE_OK;
			else
				return EHEXFILE_EOF;
		} else if (
				c != '\n' 
			&& 	c != '\t' 
			&& 	c != '\r' 
			&& 	c != ' ') {
			return EHEXFILE_BADLINE;
		}
	} while (c != '\n');
			
	return EHEXFILE_OK;
}

static const char hexdigits[] = "0123456789ABCDEF";

#define HEXFILE_WRITEHEX(f, h) {			\
	fputc(hexdigits[ ((h) & 0xF0) >> 4], f);		\
	fputc(hexdigits[ (h) & 0x0F], f);			\
	checksum += ((h) & 0xFF);				\
}
	

int hexfile_writeline(
	FILE *fp
,	void *buf
,	unsigned int type
,	unsigned int addr
,	unsigned int len) {
	
	unsigned char *buf2 = (unsigned char *)buf;

	unsigned int checksum = 0;
	int i;
	
	fputc(':', fp);
	
	if (len>0xFF
	|| addr>0xFFFF
	|| type>0xFF)
		return EHEXFILE_BADLINE;
		
	HEXFILE_WRITEHEX(fp, len);
	
	HEXFILE_WRITEHEX(fp, (addr & 0xFF00) >> 8);
	HEXFILE_WRITEHEX(fp, addr & 0xFF);
	
	HEXFILE_WRITEHEX(fp, type);
	
	for (i=0;i<len;i++) {
		HEXFILE_WRITEHEX(fp, buf2[i]);
	}
	
	HEXFILE_WRITEHEX(fp, 1 + ~checksum);
	
	fputc('\n', fp);
	
	return EHEXFILE_OK;
		
}


char *hexfile_errorstring(int errornumber) {
	switch (errornumber) {
		case EHEXFILE_OK:	return "OK";
		case EHEXFILE_EOF:	return "unexpected end of file, no EOF marker?";
		case EHEXFILE_BADLINE:	return "badly formatted line";
		case EHEXFILE_CKSUM:	return "bad checksum!";
		default:		return "unexpected error - contact developer";
	}
}

