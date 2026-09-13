#ifndef __REQUEST_H__
#define __REQUEST_H__

void request_handle(int fd);
int request_parse_uri(char *uri, char *filename, char *cgiargs);
int request_peek_filesize(int fd); /* SFF: MSG_PEEK + stat; -1 if unknown */

#endif