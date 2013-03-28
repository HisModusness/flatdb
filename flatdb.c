//
//  flatdb.c
//  flatdb
//
//  Created by Liam Westby on 3/10/13.
//  All necessary functions for working with the flatdb using system calls.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "flatdb.h"

#define E_SEEK "An error occurred while seeking in the file.\n"
#define E_WRITE_STRUCT "Something happened and only part of the person was written, or the person was not written.\n"
#define E_READ_STRUCT "Something happened while reading the Person in, and not all of the person was read in.\n"
#define E_WRITE_ENTRY "Something was wrong with this entry.\n"

#define M_DUPLICATE "An entry with that ID is already stored.\n"
#define M_NOT_FOUND "That name was not found in the database.\n"

/* Add an entry to the database. The offset is returned to its location after this method runs.
 * No duplicate entries (as decided by the ID) are allowed.
 *
 * fd: The file descriptor to write to.
 * person: The person to write to the database.
 */
void db_add(char *filepath, Person *person) {
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd < 3) {
        printf("The file %s was not found.\n", filepath);
        exit(1);
    }
    
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
    
    // Print the name and id of the person added.
    char person_data[100];
    sprintf(person_data, "id: %d, name: %s added.\n", person->id, person->name);
    write(1, person_data, strlen(person_data));
    
    // For predictible behavior, seek to the beginning.
    if (lseek(fd, 0, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    
    close(fd);
}

/* Get the ID associated with a given name. The cursor is left just before the matching record, 
 * or at the end of the file if not found. Returns the first ID associated with the name.
 *
 * fd: File descriptor to read from.
 * name: Name to look for.
 *
 * Returns: ID of the person if found, -1 if not.
 */
int db_get(char *filepath, char *name) {
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd < 3) {
        printf("The file %s was not found.\n", filepath);
        exit(1);
    }
    
    // Start at the beginning.
    if (lseek(fd, 0, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    Person *to_check = db_get_current_record(fd);
    
    while (to_check != NULL) {
        if (!strcmp(to_check->name, name)) return to_check->id;
        
        // Since we successfully read the record in, we know we can skip over it fine.
        db_seek_record(fd);
        if (db_eof(fd)) break;
        to_check = db_get_current_record(fd);
    }
    return -1;
    
    close(fd);
}

/* Remove the first person found with the given name.
 *
 * fd: File descriptor to read from.
 * name: Name to look for.
 */
void db_remove(char *filepath, char *name) {
    if (db_get(filepath, name) == -1) {
        write(1, M_NOT_FOUND, strlen(M_NOT_FOUND));
        return;
    }
    
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd < 3) {
        printf("The file %s was not found.\n", filepath);
        exit(1);
    }
    
    // Now we not only know where it is, we're positioned right before it.
    Person *to_remove = db_get_current_record(fd);
    
    // Must get to the last record.
    off_t removed_record_position = lseek(fd, 0, SEEK_CUR);
    
    // We can easily find the new size of the file by seeking to just before the last record.
    off_t new_size = lseek(fd, -sizeof(Person), SEEK_END);
    Person *replacement = db_get_current_record(fd);
    
    // We now go to move the last record into the deleted record's position.
    lseek(fd, removed_record_position, SEEK_SET);
    if (write(fd, replacement, sizeof(Person)) < sizeof(Person)) {
        write(2, E_WRITE_STRUCT, strlen(E_WRITE_STRUCT));
    }
    
    // Bring the file down to its new size.
    lseek(fd, removed_record_position, SEEK_SET);
    ftruncate(fd, new_size);
    
    char person_data[100];
    sprintf(person_data, "id: %d, name: %s removed.\n", to_remove->id, to_remove->name);
    write(1, person_data, strlen(person_data));
    
    close(fd);
}

/* Print the database to standard output.
 *
 * fd: File descriptor to read from.
 */
void db_print(char *filepath) {
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd < 3) {
        printf("The file %s was not found.\n", filepath);
        exit(1);
    }
    
    off_t position = lseek(fd, 0, SEEK_CUR);
    
    // Start at the beginning.
    if (lseek(fd, 0, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    write(1, "Contents:\n", sizeof("Contents\n"));
    Person *to_write = db_get_current_record(fd);
    
    while (to_write != NULL) {
        char output_line[100];
        
        // sprintf is like printf but stores the result in a string.
        sprintf(output_line, "id: %d, name: %s\n", to_write->id, to_write->name);
        
        if (write(1, output_line, strlen(output_line)) < strlen(output_line)) {
            write(2, E_WRITE_ENTRY, strlen(E_WRITE_ENTRY));
        }
        
        // Since we successfully read the record in, we know we can skip over it fine.
        db_seek_record(fd);
        if (db_eof(fd)) break;
        to_write = db_get_current_record(fd);
    }
    
    // Return to where the pointer was.
    if (lseek(fd, position, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
}

/* Read a record in from the file. Assumes the fd is at the beginning of a record.
 * Returns the pointer back to where it started from.
 *
 * fd: File descriptor representing the file to read
 *
 * Returns: a Person struct with the values populated as stored
 */
Person* db_get_current_record(int fd) {
    Person *result = (Person*)malloc(sizeof(Person));
    
    if (read(fd, result, sizeof(Person)) < sizeof(Person)) {
        //write(2, E_READ_STRUCT, strlen(E_READ_STRUCT));
        return NULL;
    }
    
    // Uphold the contract of the method; seek to where the cursor was previously.
    if (lseek(fd, -sizeof(Person), SEEK_CUR) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
    }
    
    return result;
}

/* Seek past one record in the database.
 *
 * fd: File descriptor to seek.
 */
void db_seek_record(int fd) {
    // Seek along Person lines, to keep pointer aligned to data.
    if (lseek(fd, sizeof(Person), SEEK_CUR) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
}

/* Find out if the file descriptor is pointing to the end of the file.
 *
 * fd: File descriptor to read from.
 *
 * Returns: 1 if at or after EOF, 0 otherwise.
 */
int db_eof(int fd) {
    if (read(fd, NULL, 1) == 0) return 1;
    return 0;
}

/* Determine if the given ID exists in the database.
 *
 * fd: File descriptor to read from.
 * id: The ID to look for.
 *
 * Returns: 1 if found, 0 if not.
 */
int db_id_exists(int fd, int id) {
    // Save the position of the pointer.
    off_t position = lseek(fd, 0, SEEK_CUR);
    
    // Start at the beginning.
    if (lseek(fd, 0, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    Person *to_check = db_get_current_record(fd);
    
    while (to_check != NULL) {
        if (to_check->id == id) {
            if (lseek(fd, position, SEEK_SET) == -1) {
                write(2, E_SEEK, strlen(E_SEEK));
                exit(1);
            }
            return 1;
        }
        
        // Since we successfully read the record in, we know we can skip over it fine.
        db_seek_record(fd);
        if (db_eof(fd)) break;
        to_check = db_get_current_record(fd);
    }
    if (lseek(fd, position, SEEK_SET) == -1) {
        write(2, E_SEEK, strlen(E_SEEK));
        exit(1);
    }
    return 0;
}