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

#define E_SEEK "An error occurred while seeking in the file.\n"
#define E_WRITE_STRUCT "Something happend and only part of the person was written, or the person was not written. Exiting.\n"
#define E_READ_STRUCT "Something happend while reading the Person in, and not all of the person was read in. Exiting.\n"

void db_add(int fd, Person *person) {
    
    // We are to append to the end of the file
    if (lseek(fd, 0, SEEK_END) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    
    // Through the magic of television, all we really have to do is write the struct out
    // The char array is allocated statically, so it is written as well
    if (write(fd, person, sizeof(Person)) < sizeof(Person)) {
        write(2, E_WRITE_STRUCT, strlen(E_WRITE_STRUCT));
        exit(1);
    }
    
    // For predictible behavior, seek to the beginning.
    if (lseek(fd, 0, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
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
    Person result;
    
    if (read(fd, &result, sizeof(Person)) < sizeof(Person)) {
        write(2, E_READ_STRUCT, strlen(E_READ_STRUCT));
        exit(1);
    }
    
    // Uphold the contract of the method; seek to where the cursor was previously.
    if (lseek(fd, -sizeof(Person), SEEK_CUR) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    
    return result;
}

void db_seek_record(int fd) {
    // Seek along Person lines, to keep pointer aligned to data.
    if (lseek(fd, sizeof(Person), SEEK_CUR) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
}