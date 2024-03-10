#ifndef SYNC_H
# define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_PATH_LENGTH 1024

extern int log_fd;

enum args_index
{
	SOURCE = 1,
	REPLICA,
	LOGS
};

bool error_exit(const char *message);
bool synchronize_folders(char *source_path, char *replica_path, char *log_file_path);

#endif

