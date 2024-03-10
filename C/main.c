#include "sync.h"

bool error_exit(const char *message)
{
	write(STDERR_FILENO, message, strlen(message));
	return false;
}

static bool validate_arguments(int ac, char **av)
{
	if (ac != 4)
	{
		error_exit("[validate_arguments] - Invalid number of arguments\n");
		return error_exit("Usage: ./<executable> <source_directory> <replica_directory> <log_file>\n");
	}
	if (strlen(av[1]) > MAX_PATH_LENGTH ||
		strlen(av[2]) > MAX_PATH_LENGTH ||
		strlen(av[3]) > MAX_PATH_LENGTH)
	{
		return error_exit("[validate_arguments] - Path length exceeds maximum length\n");
	}
	if (access(av[1], F_OK) != 0)
	{
		error_exit("[validate_arguments] - Source directory does not exist: ");
		return error_exit(strcat(av[1], "\n"));
	}
	log_fd = open(av[3], O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (log_fd == -1)
	{
		error_exit("[validate_arguments] - Failed to open log file: ");
		return error_exit(strcat(av[3], "\n"));
	}
	return true;
}

int log_fd = -1;

int main(int ac, char **av)
{
	if (!validate_arguments(ac, av))
		return EXIT_FAILURE;
	if (!synchronize_folders(av[SOURCE], av[REPLICA], av[LOGS])) {
		if (log_fd != -1)
			close(log_fd);
		return EXIT_FAILURE;
	}
	close(log_fd);
	exit(EXIT_SUCCESS);
}
