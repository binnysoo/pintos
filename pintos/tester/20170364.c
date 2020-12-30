#include "20170364.h"

struct list * find_named_list(named_list *head, char *name) {
	named_list *tmp_list = head;
	while(tmp_list->next != NULL) {
		tmp_list = tmp_list->next;
		if (!strcmp(tmp_list->name, name)) {
			return &tmp_list->matching_list;
		}
	}
}

struct list_elem * find_elem_by_index(struct list *l, int idx) {
	int cnt = 0;
	for (struct list_elem *e = list_begin(l); e!=list_end(l); e=list_next(e)) {
		if (cnt++ == idx) {
			return e;
		}
	}
}

struct hash * find_named_hash(named_hash *head, char *name) {
	named_hash *tmp_hash = head;
	while(tmp_hash->next != NULL) {
		tmp_hash = tmp_hash->next;
		if (!strcmp(tmp_hash->name, name)) {
			return &tmp_hash->matching_hash;
		}
	}
}

struct bitmap * find_named_bitmap(named_bitmap *head, char *name) {
	named_bitmap *tmp_btmp = head;
	while(tmp_btmp->next != NULL) {
		tmp_btmp = tmp_btmp->next;
		if (!strcmp(tmp_btmp->name, name)) {
			return tmp_btmp->matching_bitmap;
		}
	}
}

command * get_cmd() {
	int arg_idx = 0;
	char *token;
	char buffer[MAX_CMD];
	command *new_cmd = (command *)malloc(sizeof(command));
	new_cmd->cmd = NULL;
	new_cmd->argnum = 0;
	for (int i = 0; i < MAX_ARG; i++) new_cmd->arg[i] = '\0';

	fgets(buffer, MAX_CMD, stdin);
	token = strtok(buffer, " ");
	if (token[strlen(token) - 1] == '\n') {
		token[strlen(token) - 1] = '\0';
	}
	new_cmd->cmd = (char*)malloc(strlen(token) + 1);
	strcpy(new_cmd->cmd, token);

	token = strtok(NULL, " ");
	while (token != NULL) {
		if (token[strlen(token) - 1] == '\n') {
			token[strlen(token) - 1] = '\0';
		}
		new_cmd->arg[arg_idx] = (char*)malloc(strlen(token) + 1);
		strcpy(new_cmd->arg[arg_idx++], token);
		token = strtok(NULL, " ");
	}
	new_cmd->argnum = arg_idx;

	return new_cmd;	 
}

void free_cmd(command *new_cmd) {
	free(new_cmd->cmd);
	for (int i = 0; i<new_cmd->argnum; i++) {
		free(new_cmd->arg[i]);
	}
}

datatype check_datatype(name_map *names, char *name) {
	named_list *tmplist = names->list_head;
	named_hash *tmphash = names->hash_head;
	named_bitmap *tmpbtmp = names->bitmap_head;
	/* check the given data structure */
	/* list */
	while(tmplist->next != NULL) {
		tmplist = tmplist->next;
		if (strcmp(tmplist->name, name) == 0)
			return LIST;
	}
	/* hash */
	while(tmphash->next != NULL) {
		tmphash = tmphash->next;
		if (strcmp(tmphash->name, name) == 0)
			return HASH;
	}
	/* bitmap */
	while(tmpbtmp->next != NULL) {
		tmpbtmp = tmpbtmp->next;
		if (strcmp(tmpbtmp->name, name) == 0)
			return BITMAP;
	}
	return NOT_FOUND;
}

void call_create(command *new_cmd, name_map *names) {
	/* creat struct list */
	if (strcmp(new_cmd->arg[0], "list") == 0) {
		named_list *new_list = (named_list *)malloc(sizeof(named_list));
		named_list *tmp_list = names->list_head;

		new_list->name = (char*)malloc(strlen(new_cmd->arg[1]) + 1);
		strcpy(new_list->name, new_cmd->arg[1]);
		new_list->next = NULL;

		list_init(&new_list->matching_list);

		while(tmp_list->next != NULL) {
			tmp_list = tmp_list->next;
		}
		tmp_list->next = new_list;
	}
	else if (strcmp(new_cmd->arg[0], "hashtable") == 0) {
		named_hash *new_hash = (named_hash *)malloc(sizeof(named_hash));
		named_hash *tmp_hash = names->hash_head;

		new_hash->name = (char*)malloc(strlen(new_cmd->arg[1] + 1));
		strcpy(new_hash->name, new_cmd->arg[1]);
		new_hash->next = NULL;

		hash_init(&new_hash->matching_hash, hash_func, hash_less, NULL);

		while(tmp_hash->next != NULL) {
			tmp_hash = tmp_hash->next;
		}
		tmp_hash->next = new_hash;
	}
	else if (strcmp(new_cmd->arg[0], "bitmap") == 0) {
		named_bitmap *new_btmp = (named_bitmap *)malloc(sizeof(named_bitmap));
		named_bitmap *tmp_btmp = names->bitmap_head;

		new_btmp->name = (char*)malloc(strlen(new_cmd->arg[1] + 1));
		strcpy(new_btmp->name, new_cmd->arg[1]);
		new_btmp->next = NULL;
	
		new_btmp->matching_bitmap = bitmap_create(atoi(new_cmd->arg[2]));

		while(tmp_btmp->next != NULL) {
			tmp_btmp = tmp_btmp->next;
		}
		tmp_btmp->next = new_btmp;
	}
	else err_arg_not_valid(new_cmd->arg[0]);
}

void call_delete(command *new_cmd, name_map *names) {
	/* check the given data structure */
	/* list */
	named_list *tmplist = names->list_head;
	named_list *prevlist;
	while(tmplist->next != NULL) {
		prevlist = tmplist;
		tmplist = tmplist->next;
		if (!strcmp(tmplist->name, new_cmd->arg[0])) {
			prevlist->next = tmplist->next;
			while (!list_empty (&tmplist->matching_list)) {
				struct list_elem *e = list_pop_front (&tmplist->matching_list);
			}
			free(tmplist->name);
			free(tmplist);
			return;
		}
	}
	/* hash */
	named_hash *tmphash = names->hash_head;
	named_hash *prevhash;
	while(tmphash->next != NULL) {
		prevhash = tmphash;
		tmphash = tmphash->next;
		if (!strcmp(tmphash->name, new_cmd->arg[0])) {
			prevhash->next = tmphash->next;
			
			hash_destroy(&tmphash->matching_hash, hash_action_destructor);
			
			free(tmphash->name);
			free(tmphash);
			return;
		}
	}
	/* bitmap */
	named_bitmap *tmpbtmp = names->bitmap_head;
	named_bitmap *prevbtmp;
	while(tmpbtmp->next != NULL) {
		prevbtmp = tmpbtmp;
		tmpbtmp = tmpbtmp->next;
		if (!strcmp(tmpbtmp->name, new_cmd->arg[0])) {
			prevbtmp->next = tmpbtmp->next;

			bitmap_destroy(tmpbtmp->matching_bitmap);

			free(tmpbtmp->name);
			free(tmpbtmp);
			return;
		}
	}
	err_name_not_found(new_cmd->arg[0]);
	return;
}

void call_dumpdata(command *new_cmd, name_map *names) {
	/* check the given data structure */
	/* list */
	named_list *tmplist = names->list_head;
	named_list *prevlist;
	while(tmplist->next != NULL) {
		prevlist = tmplist;
		tmplist = tmplist->next;
		if (strcmp(tmplist->name, new_cmd->arg[0]) == 0) {
			/* print out data */	
			struct list *l = &tmplist->matching_list;
			for (struct list_elem *e = list_begin(l); e != list_end(l); e = list_next(e)) {
				struct list_item *item = list_entry(e, struct list_item, elem);
				printf("%d ", item->data);
			}
			if (!list_empty(l)) printf("\n");
			return;
		}
	}
	/* hash */
	named_hash *tmphash = names->hash_head;
	named_hash *prevhash;
	while(tmphash->next != NULL) {
		prevhash = tmphash;
		tmphash = tmphash->next;
		if (strcmp(tmphash->name, new_cmd->arg[0]) == 0) {
			/* print out data */
			struct hash *h = &tmphash->matching_hash;
			struct hash_iterator i;
			hash_first (&i, h);
			while(hash_next(&i)) {
				struct hash_item *item = hash_entry (hash_cur(&i), struct hash_item, elem);
				printf("%d ", item->data);
			}
			if (!hash_empty(h)) printf("\n");
			return;
		}
	}
	/* bitmap */
	named_bitmap *tmpbtmp = names->bitmap_head;
	named_bitmap *prevbtmp;
	while(tmpbtmp->next != NULL) {
		prevbtmp = tmpbtmp;
		tmpbtmp = tmpbtmp->next;
		if (strcmp(tmpbtmp->name, new_cmd->arg[0]) == 0) {
			/* print out data */
			struct bitmap *b = tmpbtmp->matching_bitmap;

			for (int i=0;i<bitmap_size(b); i++)
				printf("%d", bitmap_test(b, i));
			if (bitmap_size(b) != 0) printf("\n");
			return;
		}
	}
}

void call_list_push(command *new_cmd, struct list *l) {
	struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
	if (!strcmp(new_cmd->cmd, "list_push_front")) {
		list_push_front(l, e);
	}
	else if (!strcmp(new_cmd->cmd, "list_push_back")) {
		list_push_back(l, e);
	}
	struct list_item *item = list_entry(e, struct list_item, elem);
	item->data = atoi(new_cmd->arg[1]);

	return;
}

void call_list_pop(command *new_cmd, struct list *l) {
	if (!strcmp(new_cmd->cmd, "list_pop_front")) {
		list_pop_front(l);
	}
	else if (!strcmp(new_cmd->cmd, "list_pop_back")) {
		list_pop_back(l);
	}
	return;
}


/* MAIN FUNCTION */
/* get command from the user and run function */
int main() {
	command *new_cmd;
	name_map names;

	/* initialize names */
	names.list_head = (named_list *)malloc(sizeof(named_list));
	names.hash_head = (named_hash *)malloc(sizeof(named_hash));
	names.bitmap_head = (named_bitmap *)malloc(sizeof(named_bitmap));
	
	names.list_head->next = NULL;
	names.hash_head->next = NULL;
	names.bitmap_head->next = NULL;

	while(1) {
		new_cmd = get_cmd();
		if (!strcmp(new_cmd->cmd, "quit")) {
			break;
		}
		else if (!strcmp(new_cmd->cmd, "create")) {
			call_create(new_cmd, &names);
		}
		else if (!strcmp(new_cmd->cmd, "delete")) {
			call_delete(new_cmd, &names);
		}
		else if (!strcmp(new_cmd->cmd, "dumpdata")) {
			call_dumpdata(new_cmd, &names);
		}
		else if (!strncmp(new_cmd->cmd, "list_", strlen("list_"))) {
			struct list *l = find_named_list(names.list_head, new_cmd->arg[0]);
			if (!strncmp(new_cmd->cmd, "list_push", strlen("list_push"))) {
				call_list_push(new_cmd, l);
			}
			else if (!strncmp(new_cmd->cmd, "list_pop", strlen("list_pop"))) {
				call_list_pop(new_cmd, l);
			}
			else if (!strcmp(new_cmd->cmd, "list_front")) {
				struct list_elem *e = list_front(l);
				printf("%d\n", list_entry(e, struct list_item, elem)->data);
			}
			else if (!strcmp(new_cmd->cmd, "list_back")) {
				struct list_elem *e = list_back(l);
				printf("%d\n", list_entry(e, struct list_item, elem)->data);
			}
			else if (!strcmp(new_cmd->cmd, "list_min")) {
				struct list_elem *e = list_min(l, list_less, NULL);
				printf("%d\n", list_entry(e, struct list_item, elem)->data);
			}
			else if (!strcmp(new_cmd->cmd, "list_max")) {
				struct list_elem *e = list_max(l, list_less, NULL);
				printf("%d\n", list_entry(e, struct list_item, elem)->data);
			}
			else if (!strcmp(new_cmd->cmd, "list_empty")) {
				printf("%s\n", list_empty(l) ? "true" : "false");
			}
			else if (!strcmp(new_cmd->cmd, "list_insert")) {
				struct list_elem *before = find_elem_by_index(l, atoi(new_cmd->arg[1]));
				struct list_elem *new = (struct list_elem *)malloc(sizeof(struct list_elem));
				list_insert(before, new);
				struct list_item *item = list_entry(new, struct list_item, elem);
				item->data = atoi(new_cmd->arg[2]);
			}
			else if (!strcmp(new_cmd->cmd, "list_insert_ordered")) {
				struct list_elem *new = (struct list_elem *)malloc(sizeof(struct list_elem));
				struct list_item *item = list_entry(new, struct list_item, elem);
				item->data = atoi(new_cmd->arg[1]);
				list_insert_ordered(l, new, list_less, NULL);
			}
			else if (!strcmp(new_cmd->cmd, "list_remove")) {
				struct list_elem *e = find_elem_by_index(l, atoi(new_cmd->arg[1]));
				list_remove(e);
			}
			else if (!strcmp(new_cmd->cmd, "list_reverse")) {
				list_reverse(l);
			}
			else if (!strcmp(new_cmd->cmd, "list_shuffle")) {
				list_shuffle(l);
			}
			else if (!strcmp(new_cmd->cmd, "list_size")) {
				printf("%zu\n", list_size(l));
			}
			else if (!strcmp(new_cmd->cmd, "list_sort")) {
				list_sort(l, list_less, NULL);	
			}
			else if (!strcmp(new_cmd->cmd, "list_splice")) {
				struct list_elem *before = find_elem_by_index(l, atoi(new_cmd->arg[1]));
				struct list *l2 = find_named_list(names.list_head, new_cmd->arg[2]);
				struct list_elem *first = find_elem_by_index(l2, atoi(new_cmd->arg[3]));
				struct list_elem *last = find_elem_by_index(l2, atoi(new_cmd->arg[4]));
				list_splice(before, first, last);
			}
			else if (!strcmp(new_cmd->cmd, "list_swap")) {
				int idx1 = atoi(new_cmd->arg[1]);
				int idx2 = atoi(new_cmd->arg[2]);
				
				if (idx1 > idx2) {
					idx1 = atoi(new_cmd->arg[2]);
					idx2 = atoi(new_cmd->arg[1]);
				}
				
				struct list_elem *e1 = find_elem_by_index(l, idx1);
				struct list_elem *e2 = find_elem_by_index(l, idx2);
				list_swap(e1, e2);
			}
			else if (!strcmp(new_cmd->cmd, "list_unique")) {
				struct list *dup = NULL;
				if (new_cmd->argnum == 2) {
					dup = find_named_list(names.list_head, new_cmd->arg[1]);
				}
				list_unique(l, dup, list_less , NULL);
			}
			else {
				err_cmd_not_valid(new_cmd->cmd);
			}
		}
		else if (!strncmp(new_cmd->cmd, "hash_", strlen("hash_"))) {
			struct hash *h = find_named_hash(names.hash_head, new_cmd->arg[0]);
			if (!strcmp(new_cmd->cmd, "hash_insert")) {
				struct hash_elem *new = (struct hash_elem *)malloc(sizeof(struct hash_elem));
				hash_entry(new, struct hash_item, elem)->data = atoi(new_cmd->arg[1]);
			
				hash_insert(h, new);
			}
			else if (!strcmp(new_cmd->cmd, "hash_delete")) {
				struct hash_elem *tmp = (struct hash_elem *)malloc(sizeof(struct hash_elem));
				hash_entry(tmp, struct hash_item, elem)->data = atoi(new_cmd->arg[1]);
				
				hash_delete(h, tmp);

				free(tmp);	
			}
			else if (!strcmp(new_cmd->cmd, "hash_find")) {
				struct hash_elem *tmp = (struct hash_elem *)malloc(sizeof(struct hash_elem));
				hash_entry(tmp, struct hash_item, elem)->data = atoi(new_cmd->arg[1]);
				
				struct hash_elem *e = hash_find(h, tmp);
				if (e) printf("%d\n", hash_entry(e, struct hash_item, elem)->data);

				free(tmp);
			}
			else if (!strcmp(new_cmd->cmd, "hash_replace")) {
				struct hash_elem *e = (struct hash_elem *)malloc(sizeof(struct hash_elem));
				hash_entry(e, struct hash_item, elem)->data = atoi(new_cmd->arg[1]);
				hash_replace(h, e);
			}
			else if (!strcmp(new_cmd->cmd, "hash_empty")) {
				printf("%s\n", hash_empty(h) ? "true" : "false");
			}
			else if (!strcmp(new_cmd->cmd, "hash_size")) {
				printf("%zu\n", hash_size(h));
			}
			else if (!strcmp(new_cmd->cmd, "hash_clear")) {
				hash_clear(h, hash_action_destructor);
			}
			else if (!strcmp(new_cmd->cmd, "hash_apply")) {
				struct hash_iterator i;
				hash_first(&i, h);
				
				while(hash_next(&i)) {
					hash_action_apply(hash_cur(&i), new_cmd->arg[1]);
				}
			}
		}
		else if (!strncmp(new_cmd->cmd, "bitmap_", strlen("bitmap_"))) {
			struct bitmap *b = find_named_bitmap(names.bitmap_head, new_cmd->arg[0]);
			if (!strcmp(new_cmd->cmd, "bitmap_dump")) {
				bitmap_dump(b);
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_expand")) {
				bitmap_expand(b, atoi(new_cmd->arg[1]));
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_flip")) {
				bitmap_flip(b, atoi(new_cmd->arg[1]));
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_mark")) {
				bitmap_mark(b, atoi(new_cmd->arg[1]));
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_reset")) {
				bitmap_reset(b, atoi(new_cmd->arg[1]));
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_set")) {
				bool value = false;
				if (!strcmp(new_cmd->arg[2], "true")) value = true;
				bitmap_set(b, atoi(new_cmd->arg[1]), value);
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_set_all")) {
				bool value = false;
				if (!strcmp(new_cmd->arg[1], "true")) value = true;
				bitmap_set_all(b, value);
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_size")) {
				printf("%zu\n", bitmap_size(b));	
			}
			else if (!strcmp(new_cmd->cmd, "bitmap_test")) {
				printf("%s\n", bitmap_test(b, atoi(new_cmd->arg[1])) ? "true" : "false");
			}
			else {
				int start = atoi(new_cmd->arg[1]);
				int cnt = atoi(new_cmd->arg[2]);

				if (!strcmp(new_cmd->cmd, "bitmap_all")) {
					printf("%s\n", bitmap_all(b, start, cnt) ? "true" : "false");
				}
				else if (!strcmp(new_cmd->cmd, "bitmap_any")) {
					printf("%s\n", bitmap_any(b, start, cnt) ? "true" : "false");
				}
				else if (!strcmp(new_cmd->cmd, "bitmap_none")) {
					printf("%s\n", bitmap_none(b, start, cnt) ? "true" : "false");
				}
				else {
					bool value = false;
					if (!strcmp(new_cmd->arg[3], "true")) value = true;
					
					if (!strcmp(new_cmd->cmd, "bitmap_contains")) {
						printf("%s\n", bitmap_contains(b, start, cnt, value) ? "true" : "false");
					}
					else if (!strcmp(new_cmd->cmd, "bitmap_count")) {
						printf("%zu\n", bitmap_count(b, start, cnt, value));
					}
					else if (!strcmp(new_cmd->cmd, "bitmap_scan")) {
						printf("%zu\n", bitmap_scan(b, start, cnt, value));
					}
					else if (!strcmp(new_cmd->cmd, "bitmap_scan_and_flip")) {
						printf("%zu\n", bitmap_scan_and_flip(b, start, cnt, value));
					}
					else if (!strcmp(new_cmd->cmd, "bitmap_set_multiple")) {
						bitmap_set_multiple(b, start, cnt, value);
					}
				}
			}
		}
		else err_cmd_not_valid(new_cmd->cmd);

		free_cmd(new_cmd);
		free(new_cmd);
	}
	free(names.list_head);
	free(names.hash_head);
	free(names.bitmap_head);

	return 0;
}
