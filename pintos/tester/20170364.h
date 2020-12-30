/***********/
/* headers */
/***********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"

/**********/
/* macros */
/**********/
#define MAX_CMD 200
#define MAX_ARG 5 

/*******************/
/* data structures */
/*******************/

typedef enum { LIST, HASH, BITMAP, NOT_FOUND } datatype;

typedef struct _command {
	char *cmd;
	char *arg[MAX_ARG];
	int argnum;
} command;

typedef struct _named_list named_list;
struct _named_list {
	char *name;
	struct list matching_list;
	named_list *next;
};

typedef struct _named_hash named_hash;
struct _named_hash {
	char *name;
	struct hash matching_hash;
	named_hash *next;
};

typedef struct _named_bitmap named_bitmap;
struct _named_bitmap {
	char *name;
	struct bitmap *matching_bitmap;
	named_bitmap *next;
};

typedef struct _name_map {
	named_list *list_head;
	named_hash *hash_head;
	named_bitmap *bitmap_head;
} name_map;

/*************/
/* functions */
/*************/

bool list_less(const struct list_elem *a, const struct list_elem *b, void *aux) {
	int a_data = list_entry(a, struct list_item, elem)->data;
	int b_data = list_entry(b, struct list_item, elem)->data;

	return a_data < b_data;
}

bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
	int a_data = hash_entry(a, struct hash_item, elem)->data;
	int b_data = hash_entry(b, struct hash_item, elem)->data;

	return a_data < b_data;
}

unsigned hash_func(const struct hash_elem *e, void *aux) {
	return hash_int(hash_entry(e, struct hash_item, elem)->data);
}

datatype check_datatype(name_map *names, char *name);

struct list * find_named_list(named_list *, char *);
struct list_elem * find_elem_by_index(struct list *, int);

struct hash * find_named_hash(named_hash *, char *);
struct bitmap * find_named_bitmap(named_bitmap *, char *);

void free_cmd(command *);

/* error functions */
void err_cmd_not_valid(char *cmd) {
	printf("command \"%s\" not valid!!!\n", cmd);
}

void err_arg_not_valid(char *arg) {
	printf("argument \"%s\" not valid!!!\n", arg);
}

void err_name_not_found(char *name) {
	printf("data structure named \"%s\" not found!!!\n", name);
}

/* command functions */
command * get_cmd();
void call_quit(command *new_cmd, name_map *names);
void call_create(command *new_cmd, name_map *names);
void call_delete(command *new_cmd, name_map *names);
void call_dumpdata(command *new_cmd, name_map *names);

void call_list_push(command *, struct list *);
void call_list_pop(command *, struct list *);
