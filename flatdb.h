//
//  flatdb.h
//  flatdb
//
//  Created by Liam Westby on 3/10/13.
//  Person struct and function prototypes for the flatdb.
//

#ifndef flatdb_flatdb_h
#define flatdb_flatdb_h

typedef struct {
    int id;
    char name[50];
} Person;

void db_add(char *filepath, Person *person);
int db_get(char *filepath, char *name);
int db_get_remove(int fd, char *name);
void db_remove(char *filepath, char *name);
void db_print(char *filepath);
Person* db_get_current_record(int fd);
void db_seek_record(int fd);
int db_eof(int fd);
int db_id_exists(int fd, int id);

#endif
