//
//  main.c
//  flatdb
//
//  Created by Liam Westby on 3/10/13.
//  Copyright (c) 2013 Me. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "flatdb.h"

void cmd_add(int fd);
void cmd_get(int fd);
void cmd_remove(int fd);
void cmd_print(int fd);

int main(int argc, const char * argv[])
{
    printf("Welcome to FlatDB.\n");
    printf("Copyright 2013 Oracle Corporation\n");
    printf("\"We synergize your paradigms.\"\n");
    printf("\n");
    
    char path[200];
    int fd;
    do {
        printf("Enter the location of the database file. It does not need to exist; a blank one will be created for you if necessary.\n> ");
        fgets(path, 200, stdin);
        
        fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    } while (fd < 3);
    
    char command = 0;
    
    do {
        printf("\n");
        printf("Choose a command:\n");
        printf("    _a_dd an entry\n");
        printf("    _f_ind an ID\n");
        printf("    _r_remove an entry\n");
        printf("    _p_rint the database\n");
        printf("    _q_uit the application\n");
        printf("> ");
        
        command = fgetc(stdin);
        getchar();
        
        printf("\n");
        
        switch (command) {
            case 'a':
                cmd_add(fd);
                break;
                
            case 'f':
                cmd_get(fd);
                break;
                
            case 'r':
                cmd_remove(fd);
                break;
                
            case 'p':
                cmd_print(fd);
                break;
                
            default:
                break;
        }
    } while (command != 'q');
    
    printf("Thank you. Goodbye.\n");
    close(fd);
    
    return 0;
}

void cmd_add(int fd) {
    Person to_add;
    char buffer[50];
    
    printf("Enter the ID number\n> ");
    fgets(buffer, 10, stdin);
    to_add.id = atoi(buffer);
    
    printf("Enter their name\n> ");
    fgets(buffer, 49, stdin);
    
    unsigned long end = strlen(buffer) - 1;
    if (buffer[end] == '\n') buffer[end] = '\0';
    
    strcpy(to_add.name, buffer);
    
    db_add(fd, &to_add);
}

void cmd_get(int fd) {
    char buffer[100];
    printf("Enter the name to get the ID of\n> ");
    fgets(buffer, 100, stdin);
    
    unsigned long end = strlen(buffer) - 1;
    if (buffer[end] == '\n') buffer[end] = '\0';
    
    int id = db_get(fd, buffer);
    if (id > -1) {
        printf("%s has an ID of %d\n", buffer, id);
    }
    else {
        printf("%s was not found in the database.\n", buffer);
    }
}

void cmd_remove(int fd) {
    char buffer[100];
    printf("Enter the name to remove\n> ");
    fgets(buffer, 100, stdin);
    
    unsigned long end = strlen(buffer) - 1;
    if (buffer[end] == '\n') buffer[end] = '\0';
    
    db_remove(fd, buffer);
}

void cmd_print(int fd) {
    db_print(fd);
}



