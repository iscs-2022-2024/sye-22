/*
 * Copyright (C) 2014-2017 Daniel Rossier <daniel.rossier@heig-vd.ch>
 * Copyright (C) 2019-2020 Julián Burella Pérez <julian.burellaperez@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>
#include <syscall.h>

#define KB 1024
#define CHECK_MASK(x, mask) (((x) & (mask)) == (mask))

/* Convert a size in bytes to a human readable format
 * Only one number after the comma will be displayed
 */
void human_readable_size(const unsigned long int size, char *buf)
{
	char *prefix[] = {"", "K", "M", "G", "T"};
	size_t prefix_idx = 0;

	//TO COMPLETE

	double dsize = (double) size;
	while (dsize > KB) {
		dsize /= KB;
		++prefix_idx;
	}
	sprintf(buf, "%g %s", dsize, prefix[prefix_idx]);
}

/*
* Converts given mode to -rwx formatted string
*/
 void attr_to_str(mode_t mod, char *buf)
{
	//TO COMPLETE
	if (CHECK_MASK(mod, AM_RD)) {
		buf[1] = 'r';
	}

	if (CHECK_MASK(mod, AM_WR)) {
		buf[2] = 'w';
	}

	if (CHECK_MASK(mod, AM_EX)) {
		buf[3] = 'x';
	}
}

void print_info_file(const char *file, int is_dir, int extended)
{
	char date[80];
	char size[80];
	char modes[5] = "----";
	struct stat f_stat;
	struct tm *tm;

	if (!extended) {
		printf("%s%s\n", file, is_dir ? "/" : "");
	} else {
		//TO COMPLETE
		stat(file, &f_stat);
		tm = localtime((const time_t*) &f_stat.st_mtim);
		sprintf(date, "%d-%d-%d %d:%d", tm->tm_year + 1900, 
										tm->tm_mon + 1,
										tm->tm_mday,
										tm->tm_hour,
										tm->tm_min);
		if (is_dir) {
			modes[0] = 'd';
		}
		
		if (CHECK_MASK(f_stat.st_mode, AM_SYM)) {
			modes[0] = 'l';
			int fd = open(file, O_PATH | O_NOFOLLOW);
			if (fd < 0) {
				fprintf(stderr, "An error occurred while opening the symbolic link's target!\n");
				if (close(fd) < 0) {
					fprintf(stderr, "Failed to close the file descriptor!\n");
				}
				return;
			}
			char target_name[FILENAME_SIZE];
			int nread = read(fd, target_name, FILENAME_SIZE - 1);
			target_name[nread] = 0;
			strcat((char *)file, " -> ");
			strcat((char *)file, target_name);
			if (close(fd) < 0) {
				fprintf(stderr, "Failed to close the file descriptor!\n");
				return;
			}
		}
		human_readable_size(f_stat.st_size, size);
		attr_to_str(f_stat.st_mode, modes);
		printf("%s | %s | %s | %s%s\n", modes, date, size, file, is_dir ? "/" : "");
	}
}

/*
 * Main function of ls application.
 * The ls application is very very short and does not support any options like
 * -l -a etc. It is only possible to give a subdir name to list the directory
 * entries of this subdir.
 */

int main(int argc, char **argv)
{
	char *dir;
	int extended = 0;
	const char *usage = "Usage: ls [-l] [DIR]\n";

	if (argc == 1) {
		dir = ".";
	} else if ((argc > 1) && (argc < 4)) {

		if (!strcmp(argv[1], "-l")) {
			extended = 1;
			if (argc != 2) {
				dir = argv[2];
			} else {
				dir = ".";
			}

		} else if ((argc == 3) && !strcmp(argv[2], "-l")) {
			extended = 1;
			dir = argv[1];
		} else if (argc == 2) {
			dir = argv[1];
		} else {
			printf("%s", usage);
			exit(1);
		}
	} else {
		printf("%s", usage);
		exit(1);
	}

	//Print info for every file in given directory
	//TO COMPLETE. Hint: use opendir and readdir

	DIR *directory;
	struct dirent *file;
	directory = opendir(dir);

	if (directory == NULL) {
		fprintf(stderr, "Failed to open directory %s\n", dir);
		return -1;
	}

	while ((file = readdir(directory)) != NULL) {
		print_info_file(file->d_name, file->d_type == DT_DIR, extended);
	}

	if (closedir(directory) < 0) {
		fprintf(stderr, "Failed to close directory!\n");
		return -1;
	};

	exit(0);
}

