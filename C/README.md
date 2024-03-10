# Folder Synchronization Program

This program synchronizes two folders: a source folder and a replica folder. The objective is to ensure that the replica folder maintains an exact copy of the source folder.

### Solving the Task

You can implement the program in one of the following programming languages:
- Python
- C/C++
- C#

## Requirements

- Synchronization must be one-way; after synchronization, the content of the replica folder should exactly match the content of the source folder.

- Synchronization should be performed periodically.

- All file creation, copying, and removal operations should be logged to a file and to the console output.

- Folder paths, synchronization interval, and log file path should be provided using command-line arguments.

- It's discouraged to use third-party libraries for folder synchronization, but encouraged to utilize external libraries for other well-known algorithms (e.g., MD5 calculation).

## Program

**Getting Started**

To begin using the program, follow these steps:

1. Download or clone the repository to your local machine using the following command:

```shell
git clone https://github.com/SopadeGalinha/
```

The repository contains the source code and a Makefile to compile the program.

2. To build the program, navigate to the repository's root directory and execute the following command:

```shell
make
```

This command generates the `executable` file.

**How to Use**

Execute the program using the following command:

```shell
./executable <source_directory> <replica_directory> <log_file>
```

Ensure to replace `<source_directory>`, `<replica_directory>`, and `<log_file>` with the desired directories and log file path, respectively.

**Notes**

- The `<source_directory>` should contain the files and folders that you want to synchronize with the replica folder. Ensure that the path provided is correct and accessible by the program.

- The `<replica_directory>` and `<log_file>` will be created in the current directory if they don't exist.

# Program Structure

### Header File (sync.h)

This file defines necessary constants, function prototypes, and external variables used in the program.

### Source File (sync.c)

This file contains the implementation of the folder synchronization logic. It includes functions for error handling, directory validation, file copying, and synchronization.

### Main Program (main.c)

The main program validates command-line arguments, opens the log file, calls the synchronization function, and closes the log file before exiting.
