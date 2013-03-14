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
    long retval;
    
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
    retval = write(fd, id, length);
    if (retval == -1) {
        fprintf(stderr, "There was an error when writing to a file.\n");
        exit(1);
    }
    chars_to_end -= length;
    
    // Write the value separator
    retval = write(fd, VALUE_SEPARATOR, 1);
    if (retval == -1) {
        fprintf(stderr, "There was an error when writing to a file.\n");
        exit(1);
    }
    chars_to_end -= 1;
    
    // Write the name to the file
    length = strlen(person->name);
    retval = write(fd, person->name, length);
    if (retval == -1) {
        fprintf(stderr, "There was an error when writing to a file.\n");
        exit(1);
    }
    chars_to_end -= length;
    
    // Pad the line for uniform size
    while (chars_to_end > 1) {
        retval = write(fd, PAD_CHARACTER, 1);
        if (retval == -1) {
            fprintf(stderr, "There was an error when writing to a file.\n");
            exit(1);
        }
        chars_to_end -= 1;
    }
    
    // Write the record separator
    retval = write(fd, RECORD_SEPARATOR, 1);
    if (retval == -1) {
        fprintf(stderr, "There was an error when writing to a file.\n");
        exit(1);
    }
    
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

// Read a record in from the file. Assumes the fd is at the beginning of a record.
// Returns the pointer back to where it started from.
//
// fd: File descriptor representing the file to read
//
// Returns: a Person struct with the values populated as stored
Person db_get_current_record(int fd) {
    Person record;
    char id_string[11];
    int i;
    for (i = 0; i < 11; i++) {
        read(fd, &id_string[i], 1);
        if (id_string[i] == ',') {
            id_string[i] = 0;
            break;
        }
    }
    record.id = atoi(id_string);
    
    char name[LINE_WIDTH - i];
    
    int j;
    for (j = i; j < LINE_WIDTH; j++) {
        read(fd, &name[j-i], 1);
        if (name[j-i] == '\0' || name[j-i] == '\n') {
            name[j-i] = '\0';
            break;
        }
    }
    
    record.name = name;
    
    lseek(fd, -j, SEEK_CUR);
    return record;
}

void db_seek_record(int fd) {
    lseek(fd, LINE_WIDTH+1, SEEK_CUR);
}