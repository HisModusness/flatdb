//
//  flatdb.c
//  flatdb
//
//  Created by Liam Westby on 3/10/13.
//  Copyright (c) 2013 Me. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "flatdb.h"

#define LINE_WIDTH 100

void db_add(int fd, Person *person) {
    int chars_to_end = LINE_WIDTH;
    
    // Would normally use define, but write() only takes char arrays
    char VALUE_SEPARATOR[1] = {','};
    char RECORD_SEPARATOR[1] = {'\n'};
    char PAD_CHARACTER[1] = {'\0'};
    
    // Get to the end of the file so the new entry will be appended.
    lseek(fd, 0, SEEK_END);
    
    // Convert the id to a string.
    char id[10];
    sprintf(id, "%d", person->id);
    long length = strlen(id);
    
    // Write the id to the file
    write(fd, id, length);
    chars_to_end -= length;
    
    // Write the value separator
    write(fd, VALUE_SEPARATOR, 1);
    chars_to_end -= 1;
    
    // Write the name to the file
    length = strlen(person->name);
    write(fd, person->name, length);
    chars_to_end -= length;
    
    // Pad the line for uniform size
    while (chars_to_end > 1) {
        write(fd, PAD_CHARACTER, 1);
        chars_to_end -= 1;
    }
    
    // Write the record separator
    write(fd, RECORD_SEPARATOR, 1);
    
    printf("The person (id: %d, name: %s) was written to the file successfully.\n", person->id, person->name);
    
    // Reset the fd to the beginning of the file.
    lseek(fd, 0, SEEK_SET);
    
}

int db_get(int fd, char *name) {
    
}

void db_remove(int fd, char *name) {
    
}

void db_print(int fd) {
    
}