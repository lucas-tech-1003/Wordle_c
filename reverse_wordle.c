#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "constraints.h"

/* Read the wordle grid and solution from fp. 
 * Return a pointer to a wordle struct.
 * See sample files for the format. Assume the input file has the correct
 * format.  In other words, the word on each is the correct length, the 
 * words are lower-case letters, and the line ending is either '\n' (Linux,
 * Mac, WSL) or '\r\n' (Windows)
 */
struct wordle *create_wordle(FILE *fp) {
    struct wordle *w = malloc(sizeof(struct wordle));
    char line[MAXLINE]; 
    w->num_rows = 0;

    while(fgets(line, MAXLINE, fp ) != NULL) {
        
        // remove the newline character(s) 
        char *ptr;
        if(((ptr = strchr(line, '\r')) != NULL) ||
           ((ptr = strchr(line, '\n')) != NULL)) {
            *ptr = '\0';
        }
        
        strncpy(w->grid[w->num_rows], line, SIZE);
        w->grid[w->num_rows][SIZE - 1] = '\0';
        w->num_rows++;
    }
    return w;
}


/* Create a solver_node and return it.
 * If con is not NULL, copy con into dynamically allocated space in the struct
 * If con is NULL set the new solver_node con field to NULL.
 * Tip: struct assignment makes copying con a one-line statements
 */
struct solver_node *create_solver_node(struct constraints *con, char *word) {

	struct solver_node *solver = malloc(sizeof(struct solver_node));
	if (con == NULL) {			// set con to NULL
		solver -> con = NULL;
	}
	else{		// con isn't NULL
		struct constraints *new_con = malloc(sizeof(struct constraints));
		memcpy(new_con, con, sizeof(struct constraints));
		solver -> con = new_con; // copy con into solver
	}
	strncpy(solver -> word, word, 6);	// copy word into solver

	// initializes child_list and next_sibling to NULL
	solver -> child_list = NULL;
	solver -> next_sibling = NULL;
	
    return solver;
}

/* Return 1 if "word" matches the constraints in "con" for the wordle "w".
 * Return 0 if it does not match
 */
int match_constraints(char *word, struct constraints *con, 
struct wordle *w, int row) {
	// return 0 if word contains duplicate letters and the letter is in
    // the solution word
	for (int i = 0; i < WORDLEN; i++) {
		char *ptr = strchr(word, word[i]);
		if (strchr(w -> grid[0], word[i]) != NULL && 
								strchr(ptr + 1, word[i]) != NULL) {
			return 0;
		}
		// return 0 if the tile contains 'y' and the word doesn't have
		// have any letters from the solution
		char *ptr1 = strchr(w -> grid[i], 'y');
		char *ptr2 = strchr(w -> grid[i], 'g');
		if (ptr1 != NULL || ptr2 != NULL) {
			int flag = 0;
			for (int j = 0; j < WORDLEN; j++) {
				if (strchr(w -> grid[0], word[j]) != NULL) {
					flag = 1; 	// there is some letter in both word and solution
				}
			}
			if (flag == 0) {
				return 0; // there is no letters in word that should be in solution
			}
		}
	}

	// iterate through the wordle at index row
	for (int i = 0; i < WORDLEN; i++) {

		// If must_be[i] is empty.
		// So we only need to check whether word[i] is in cannot_be or not
		if (strlen((con -> must_be)[i]) == 0) {
			if ((con -> cannot_be)[word[i] - 'a'] == '1') {
				return 0;
			}
		}
		// When the tile at index i is green or yellow, must_be[i] is not empty
		// Return 0 if word[i] not in must_be[i]
		else {
			if (strchr((con -> must_be)[i], word[i]) == NULL) {
				return 0;
			} 
		}
	}
	// At this point, the word matches.
    return 1;
}
/* remove "letter" from "word"
 * "word" remains the same if "letter" is not in "word"
 */
void remove_char(char *word, char letter) {
    char *ptr = strchr(word, letter);
    if(ptr != NULL) {
        *ptr = word[strlen(word) - 1];
        word[strlen(word) - 1] = '\0';
    }
}

/* Build a tree starting at "row" in the wordle "w". 
 * Use the "parent" constraints to set up the constraints for this node
 * of the tree
 * For each word in "dict", 
 *    - if a word matches the constraints, then 
 *        - create a copy of the constraints for the child node and update
 *          the constraints with the new information.
 *        - add the word to the child_list of the current solver node
 *        - call solve_subtree on newly created subtree
 */

void solve_subtree(int row, struct wordle *w,  struct node *dict, 
                   struct solver_node *parent) {
    if(verbose) {
        printf("Running solve_subtree: %d, %s\n", row, parent->word);
    }

	// TODO
    
	// add parent->word to parent->con's cannot_be
	add_to_cannot_be(parent -> word, parent -> con);

	// update parent->must_be
	for (int i = 0; i < WORDLEN; i++) {
		strcpy((parent -> con) -> must_be[i], "");
		if ((w -> grid)[row][i] == 'g') {
			set_green((parent -> word)[i], i, parent -> con);
		}
		else if ((w -> grid)[row][i] == 'y') {
			if (row == 1) { // next_tile is all green
				set_yellow(i, (w -> grid)[row], "ggggg\0", parent -> word, 
							parent -> con);
			}
			else {
				set_yellow(i, (w -> grid)[row], (w -> grid)[row - 1], 
							parent -> word, parent -> con);
				// remove the letter that is yellow in constraint but should've
				// been green
				remove_char(((parent -> con) -> must_be)[i], (w -> grid)[0][i]);
			}
		}
		else {
			parent -> con -> must_be[i][0] = '\0';
		}
	}

	// debugging suggestion, but you can choose how to use the verbose option
    if(verbose) {
        print_constraints(parent -> con);
    }

    // TODO

	struct node *curr = dict;
	// find the first word that matches the constraints and set it to be the 
	// child of parent
	while (curr != NULL) {
        if (match_constraints(curr -> word, parent -> con, w, row) == 1) {
			// create a new solver_node for child
            struct solver_node *child = create_solver_node(parent -> con, curr -> word);
			if (parent -> child_list == NULL) {
				parent -> child_list = child;
			}
			else {
				// Need to put child into the rightest node of child
				struct solver_node *curr_child = parent -> child_list;
				while (curr_child -> next_sibling != NULL) {
					curr_child = curr_child -> next_sibling;
				}
				curr_child -> next_sibling = child;
			}

			// recursively call solve_subtree on child
			if (row < w -> num_rows - 1) {
				solve_subtree(row + 1, w, dict, child);
			}
		}

		curr = curr -> next;
	}

}

/* Print to standard output all paths that are num_rows in length.
 * - node is the current node for processing
 * - path is used to hold the words on the path while traversing the tree.
 * - level is the current length of the path so far.
 * - num_rows is the full length of the paths to print
 */

void print_paths(struct solver_node *node, char **path, 
                 int level, int num_rows) {

    // TODO

	path[level - 1] = node -> word;
	// print the path when level == num_rows
	if (level == num_rows) {
		for (int i = 0; i < num_rows; i++) {
			printf("%s ", path[i]);
		}
		printf("\n");
	}
	else {
		struct solver_node *child = node -> child_list;
		while (child != NULL) {
			print_paths(child, path, level + 1, num_rows);
			child = child -> next_sibling;
		}
	}
}

/* Free all dynamically allocated memory pointed to from w.
 */ 
void free_wordle(struct wordle *w){
    free(w);
}

/* Free all dynamically allocated pointed to from node
 */
void free_tree(struct solver_node *node){
    free_constraints(node -> con); // free the constraint
	
	if (node -> child_list == NULL) {
		free(node);
	}
	else {
		struct solver_node *child = node -> child_list;	
		while (child != NULL) {
			free_tree(child);
			child = child -> next_sibling; 
		}
		free(node);
	}
}
