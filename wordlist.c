#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"


/* Read the words from a filename and return a linked list of the words.
 *   - The newline character at the end of the line must be removed from
 *     the word stored in the node.
 *   - You an assume you are working with Linux line endings ("\n").  You are
 *     welcome to also handle Window line endings ("\r\n"), but you are not
 *     required to, and we will test your code on files with Linux line endings.
 *   - The time complexity of adding one word to this list must be O(1)
 *     which means the linked list will have the words in reverse order
 *     compared to the order of the words in the file.
 *   - Do proper error checking of fopen, fclose, fgets
 */
struct node *read_list(char *filename) {
	FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    char line[SIZE];
    struct node *first = malloc(sizeof(struct node));   // The first word
    if (fgets(line, SIZE + 1, fp) != NULL) {
        char *ptr = strchr(line, '\n');     // Finds where the newline char is
        if (ptr != NULL) {
            *ptr = '\0';
        }

        strcpy(first -> word, line);
        first -> next = NULL;
		struct node *next_node = first;
        while (fgets(line, SIZE + 1, fp) != NULL) {
            char *ptr = strchr(line, '\n');
            if (ptr != NULL) {        // Remove the newline character
                *ptr = '\0';
            }
            struct node *nodes = malloc(sizeof(struct node));     // The node preceding next_node
            strcpy(nodes -> word, line);
            next_node -> next = nodes;
            next_node = nodes;
        }
    }

	int closed = fclose(fp);
	if (closed != 0) {
		perror("fclose");
		exit(1);
	}
    return first;
}

/* Print the words in the linked-list list one per line
 */
void print_dictionary(struct node *list) {
	while (list != NULL) {
        printf("%s\n", list -> word);
        list = list -> next;
    }
}
/* Free all of the dynamically allocated memory in the dictionary list 
 */
void free_dictionary(struct node *list) {
    while (list != NULL) {
        struct node *next_node = list -> next;
        free(list);
        list = next_node;
    }
}

