/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "string.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------
void md5BytesToStr(uint8_t *md5Bytes, uint8_t md5Str[33]) {
    for (int i = 0; i < 16; i++) {
        sprintf (&md5Str[i*2], "%.02x", md5Bytes[i]);
    }
}

int
strGetLine (
    char *str,
    char *dest,
    int maxlen,
    int pos
) {
	int len = strlen(str);
	int i;
	int bufpos = 0;
	char c;

	if (pos >= len) {
        return -1;
	}

	for (i = pos ; i < len; i++)
	{
		if (bufpos >= maxlen)
			return i;

		c = str[i];
		dest[bufpos++] = str[i];

		if (c == '\n')
		{
			dest[--bufpos] = '\0';
			return i + 1;
		}
	}

	dest[bufpos] = '\0';
	return len + 1;
}

static int
wordCount(
    const char *s,
    char c,
    char lastchar
) {
	int i;
	int k;

	i = 0;
	k = 0;
	lastchar = c;
	while (s[i] != '\0') {
		if ((s[i] != c) && (lastchar == c))
			k++;
		lastchar = s[i];
		i++;
	}
	return (k);
}

static char
**tabFill(
    const char *s,
    char c,
    char **splitted,
    char lastchar
) {
	int i;
	int j;
	int k;
	char *buffer;

	buffer = (char*)malloc(sizeof(char) * strlen(s));
	if (buffer == NULL)
		return (NULL);
	i = 0;
	j = 0;
	k = 0;
	while (s[i] != '\0') {
		if ((s[i] != c) && (lastchar == c)) {
			while ((s[i] != c) && (s[i] != '\0'))
				buffer[j++] = s[i++];
			buffer[j] = '\0';
			splitted[k++] = strdup(buffer);
			j = 0;
		}
		else
			lastchar = s[i++];
	}
	return (splitted);
}

char
**strSplit(
    const char *s,
    char c
) {
	int i;
	char **splitted;
	char lastchar;

	lastchar = c;
	i = wordCount(s, c, lastchar);
	splitted = (char**)malloc(sizeof(char*) * (i + 1));
	if (splitted == NULL)
		return (NULL);
	lastchar = c;
	splitted = tabFill(s, c, splitted, lastchar);
	splitted[i] = NULL;
	return (splitted);
}

char *fileGetContents (const char *filename, int *filesize)
{
	FILE *f = NULL;
	char *ret = NULL;
	size_t _fileSize;
	int size = 0;
	if (filesize) {
	    *filesize = 0;
	}

	f = fopen(filename, "rb");

	if (!f) {
		printf("%s cannot be opened\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	_fileSize = size;

	rewind(f);

	ret = malloc(size + 1);
	size = 0;

	for (size = 0; size < _fileSize; size++) {
		ret[size] = fgetc (f);
	}

	ret[size] = '\0';

	fclose(f);

	if (filesize) {
        *filesize = _fileSize;
	}

	return ret;
}
