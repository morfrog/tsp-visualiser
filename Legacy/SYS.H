#ifndef SYS_H

void *salloc(int n_items,size_t something);
char *allocate_array(int nelements, int sizeof_element, char *array_name);
char *allocate_N_by_N_array(int n1, int n2, int sizeof_element, char *array_name);
int sfree(void *ptr);
char mygetch();
#endif
