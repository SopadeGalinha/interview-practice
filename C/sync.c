#include "sync.h"

void ft_close_dir(DIR **source_dir, DIR **replica_dir)
{
	if (*source_dir != NULL)
		closedir(*source_dir);
	if (*replica_dir != NULL)
		closedir(*replica_dir);
}

static bool validate_replica_dir(char *replica_path, char *log_file_path)
{
	DIR *replica_dir = opendir(replica_path);
	if (replica_dir == NULL)
	{
		if (access(replica_path, F_OK) == 0)
		{
			error_exit("[validate_replica_dir] - Failed to open replica directory: ");
			return error_exit(strcat(replica_path, "\n"));
		}
		else if (mkdir(replica_path, 0777) != 0)
		{
			error_exit("[validate_replica_dir] - Failed to create replica directory: ");
			return error_exit(strcat(replica_path, "\n"));
		}
	}
	closedir(replica_dir);
	return true;
}

static void log_message(const char *message, const char *log_file_path, bool printtime)
{
	time_t current_time;
	struct tm *tm_info;
	char time_str[64];

	// Get the current time
	current_time = time(NULL);
	tm_info = localtime(&current_time);
	strftime(time_str, sizeof(time_str), "[%Y-%m-%d %H:%M:%S] ", tm_info);

	// Open the log file
	if (printtime)
	{
		write(STDOUT_FILENO, time_str, strlen(time_str));
		write(log_fd, time_str, strlen(time_str));
	}
	// Write the message to the log file
	write(log_fd, message, strlen(message));

	// Also write the message to the standard output
	write(STDOUT_FILENO, message, strlen(message));
}

static char *build_path(char *entry, char *dir_name)
{
	char *path;
	char *current_dir = getcwd(NULL, 0);
	if (current_dir == NULL)
		return NULL;

	// Allocate memory for the path
	path = malloc(strlen(current_dir) + strlen(entry) + strlen(dir_name) + 3 * sizeof(char));
	if (path)
	{
		// Build the path
		strcpy(path, current_dir);
		strcat(path, "/");
		strcat(path, dir_name);
		strcat(path, "/");
		strcat(path, entry);
	}
	// Free the memory for the current directory and return the path
	free(current_dir);
	return path;
}

static bool copy_file(char *source_path, char *replica_path, char *log_file_path)
{
	int source_fd, replica_fd;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read, bytes_written;

	// Open source file for reading and replica file for writing
	source_fd = open(source_path, O_RDONLY);
	if (source_fd == -1)
	{
		error_exit("[copy_file] - Failed to open source file: ");
		return error_exit(strcat(source_path, "\n"));
	}
	replica_fd = open(replica_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (replica_fd == -1)
	{
		close(source_fd);
		error_exit("[copy_file] - Failed to open replica file: ");
		return error_exit(strcat(replica_path, "\n"));
	}

	// Copy the current file to the replica
	while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0)
	{
		bytes_written = write(replica_fd, buffer, bytes_read);
		if (bytes_written != bytes_read)
		{
			close(source_fd);
			close(replica_fd);
			error_exit("[copy_file] - Failed to write to replica file: ");
			return error_exit(strcat(replica_path, "\n"));
		}
	}

	// Check if an error occurred while reading from the source file
	if (bytes_read == -1)
	{
		error_exit("[copy_file] - Failed to read from source file: ");
		return error_exit(strcat(source_path, "\n"));
	}

	// Close the file descriptors
	close(source_fd);
	close(replica_fd);

	log_message("[copy_file] - Copied file: ", log_file_path, true);
	log_message(source_path, log_file_path, false);
	log_message(" to ", log_file_path, false);
	log_message(strcat(replica_path, "\n"), log_file_path, false);
	return true;
}

bool synchronize_folders(char *source_path, char *replica_path, char *log_file_path)
{
	DIR *source_dir, *replica_dir;
	struct dirent *entry;
	char *fullpath_src, *fullpath_rep;

	// Open the source and replica directories
	source_dir = opendir(source_path);
	if (source_dir == NULL)
	{
		ft_close_dir(&source_dir, &replica_dir);

		error_exit("[synchronize_folders] - Failed to open source directory: ");
		return error_exit(strcat(source_path, "\n"));
	}
	if (!validate_replica_dir(replica_path, log_file_path)) {
		ft_close_dir(&source_dir, &replica_dir);

		return false;
	}
	replica_dir = opendir(replica_path);
	if (replica_dir == NULL)
	{
		ft_close_dir(&source_dir, &replica_dir);

		error_exit("[synchronize_folders] - Failed to open replica directory: ");
		return error_exit(strcat(replica_path, "\n"));
	}

	// Iterate through the source directory
	while ((entry = readdir(source_dir)) != NULL)
	{
		// Skip the current and parent directories
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;

		// Build the full path for the source and replica files
		fullpath_src = build_path(entry->d_name, source_path);
		fullpath_rep = build_path(entry->d_name, replica_path);
		if (!fullpath_src || !fullpath_rep) {
			ft_close_dir(&source_dir, &replica_dir);

			return error_exit("[synchronize_folders] - Failed to allocate memory for fullpath\n");
		}

		// Get the status of the source and replica files
		struct stat source_stat, replica_stat;
		if (stat(fullpath_src, &source_stat) == -1)
		{
			ft_close_dir(&source_dir, &replica_dir);

			error_exit("[synchronize_folders] - Failed to get file status: ");
			error_exit(strcat(fullpath_src, "\n"));
			continue;
		}
		if (stat(fullpath_rep, &replica_stat) == -1)
		{
			// File does not exist in replica, so create it
			if (!copy_file(fullpath_src, fullpath_rep, log_file_path))
			{
				error_exit("[synchronize_folders] - Failed to copy file: ");
				error_exit(strcat(fullpath_src, "\n"));
				continue;
			}
		}
		else
		{
			// File exists in replica, check if it's different from source
			if (source_stat.st_mtime > replica_stat.st_mtime)
			{
				// Source file has been modified, so copy it
				if (!copy_file(fullpath_src, fullpath_rep, log_file_path))
				{
					error_exit("[synchronize_folders] - Failed to copy file: ");
					error_exit(strcat(fullpath_src, "\n"));
					continue;
				}
			}
		}
	}
	closedir(source_dir);
	closedir(replica_dir);
	return true;
}
