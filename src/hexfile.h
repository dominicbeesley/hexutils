
#ifndef	__HEXFILE__
#define	__HEXFILE__

#include <stdio.h>


#define HEXFILE_TYPE_DATA	0x00
#define HEXFILE_TYPE_EOF	0x01
#define HEXFILE_TYPE_EXTSEG	0x02
#define HEXFILE_TYPE_STARTSEG	0x03
#define HEXFILE_TYPE_EXTLIN	0x04
#define HEXFILE_TYPE_STARTLIN	0x05

#define	EHEXFILE_OK		0
#define	EHEXFILE_EOF		-1	/* unexpected eof in middle of data */
#define EHEXFILE_BADLINE	-2	/* unrecognised line, bad format, etc*/
#define EHEXFILE_CKSUM		-3	/* bad checksum for line */

int hexfile_readline(
	FILE *fp
,	void *buf
,	unsigned int *type
,	unsigned int *addr
,	unsigned int *len);	/* returns errors as above */

int hexfile_writeline(
	FILE *hex
,	void *buf
,	unsigned int type
,	unsigned int addr
,	unsigned int len);	/* returns errors as above */

char *hexfile_errorstring(int errornumber);

#endif
