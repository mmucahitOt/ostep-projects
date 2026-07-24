#ifndef PATH_H
#define PATH_H

void init_search_path(void);
char *find_command(char *command);
void path_command(char **dirs);

#endif
