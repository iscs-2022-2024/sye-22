#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>


#define LETTERS_NB 26
#define A_UPPER_OFFSET 65
#define A_LOWER_OFFSET 97
#define MAX_THREAD_NUM 15
#define STR(x) #x
#define XSTR(x) STR(x)

typedef struct  {
    /* Pointer to position in the text */
	char *text_pointer;
    /* Size of the text chuck to process */
	size_t size;
    /* Letters counters, used to send the results back */
	size_t counters[LETTERS_NB];
} count_param_t;


/**
 * @brief Threads function used to count the letters occurence in a text
 */
void *count_letters(void *args) {	
	/* TO COMPLETE */
	count_param_t *arg = (count_param_t *)(args);
	for (int i = 0; i < arg->size; ++i) {
		char c = arg->text_pointer[i];
		if ((isalpha(c))) {
			arg->counters[tolower(c) - A_LOWER_OFFSET]++;
		}
	}
	return NULL;
}

/**
 * @brief Calculate the size of a file
 * 
 * @return The size of the file <fp>
 */
size_t fseek_filesize(FILE *fp) {
    long off;


    if (fseek(fp, 0, SEEK_END) == -1)
    {
        printf("failed to fseek\n");
        exit(EXIT_FAILURE);
    }

    off = ftell(fp);
    if (off == -1)
    {
        printf("failed to ftell\n");
        exit(EXIT_FAILURE);
    }

	fseek(fp, 0, SEEK_SET);

	return off;
}

/**
 * @brief Load a file content into a char buffer
 * 
 * @return A char *buffer with the content of the file <filename>
 */
char *buffer_from_file(char *filename) {

	char *buffer;
	size_t text_size = 0;
	FILE *f = fopen(filename, "r");

	if (f == NULL) {
        printf("failed to fopen %s\n", filename);
        exit(EXIT_FAILURE);
	}

	text_size = fseek_filesize(f);

	buffer = malloc(text_size+1);

	if (buffer == NULL) {
        printf("failed to malloc() the buffer\n");
        exit(EXIT_FAILURE);
	} 

	/* Read the whole content into buffer */
	fread(buffer, 1, text_size, f);

	fclose(f);

	/* Add the \0 at the end */
	buffer[text_size] = 0;

	return buffer;
}


/**
 * @brief Main function
 */
int main(int argc, char **argv)
{

	int thread_num;			// Will contain the numnber of threads we want to launch

	count_param_t *params;	// Will contain the parameters passed to the threads
	pthread_t *threads;		// Array of pthread used to join them

	size_t chunk_size = 0; 	// Will contain the chunk size each thread will process 
	size_t leftovers = 0;  	// Will contain the lefotvers not covered by the chunks
	size_t text_size;		// Will contain the text size
	char *text;				// Will contain the text itself as (char *) buffer

	size_t result_counters[LETTERS_NB] = { 0 }; // Used to compile the final count result

	char *filename;			// Will contain the filename of the file we want to count the characters


	/* Arguments checks */
	
	/* TO COMPLETE */
	if (argc != 3) {
		printf("usage: threads <thread count> <filename>\n");
		fflush(stdout);
		return EXIT_FAILURE;
	}

	thread_num = atoi(argv[1]);	
	if (thread_num <= 0 || thread_num > MAX_THREAD_NUM) {
		printf("thread count must be greater than 0 and less than " XSTR(MAX_THREAD_NUM) "\n");
		return EXIT_FAILURE;
	}
	filename = argv[2];

	/* End argument check */


	/* Load text from file */
	text = buffer_from_file(filename);
	text_size = strlen(text);

	/* Text split preparation */

	/* TO COMPLETE */
	chunk_size = text_size / thread_num;
	leftovers = text_size % thread_num;
	params = (count_param_t *)calloc(thread_num, sizeof(count_param_t));
	
	for (int i = 0; i < thread_num; ++i) {
		count_param_t count_param = {
			.text_pointer = text + i * chunk_size,
			.size = i == thread_num - 1 ? chunk_size + leftovers : chunk_size,
		};

		params[i] = count_param;
	}

	/* End text split preparation */

	printf("Now counting on a text of size %d, with %d threads each counting %d char\n", text_size, thread_num, chunk_size);
	
	/* Threads creation and parameters setting */


	/* TO COMPLETE */
	threads = (pthread_t *)calloc(thread_num, sizeof(pthread_t));
	for (int i = 0; i < thread_num; ++i) {
		pthread_t tid;
		pthread_create(&tid, NULL, (void *)count_letters, &params[i]);
		threads[i] = tid; 
	}

	/* End threads creation and parameters setting */

	/* Thread join and result gathering */

	/* TO COMPLETE */
	for (int i = 0; i < thread_num; ++i) {
		pthread_join(threads[i], NULL);
	}
	for (int i = 0; i < thread_num; ++i) {
		for (int j = 0; j < LETTERS_NB; ++j) {
			result_counters[j] += params[i].counters[j];
		}
	}
	/* End thread join and result gathering */
	

	/* Display the results */
	for (int i = 0; i < LETTERS_NB; ++i) {
		printf("Number of %c: %u\n", (char)(i+A_LOWER_OFFSET), result_counters[i]);
	}
	
	/* Free the memory */
	free(text);

	/* MAYBE TO COMPLETE? */
	free(params);
	free(threads);

	/* End free the memory */

	return EXIT_SUCCESS;
}
