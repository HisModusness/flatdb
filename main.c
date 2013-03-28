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

void cmd_add(char *filepath);
void cmd_get(char *filepath);
void cmd_remove(char *filepath);
void cmd_print(char *filepath);

void print_usage();
void run_interactive();
void run_multiprocess(int argc, const char **argv);
void run_worker(int argc, const char **argv);

int main(int argc, const char * argv[])
{
    if (argc < 2) {
        print_usage();
        return 0;
    }
    
    if (argc == 2) {
        if (!strcmp("-i", argv[1])) {
            run_interactive();
            return 0;
        }
        
        print_usage();
        return 0;
    }
    
    if (argc == 3) {
        if (!strcmp("-d", argv[1])) {
            run_multiprocess(argc, argv);
            return 0;
        }
        
        if (!strcmp("-w", argv[1])) {
            run_worker(argc, argv);
            return 0;
        }
        
        print_usage();
        return 0;
    }
    
    print_usage();
    return 0;
}

void cmd_add(char *filepath) {
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
    
    db_add(filepath, &to_add);
}

void cmd_get(char *filepath) {
    char buffer[100];
    printf("Enter the name to get the ID of\n> ");
    fgets(buffer, 100, stdin);
    
    unsigned long end = strlen(buffer) - 1;
    if (buffer[end] == '\n') buffer[end] = '\0';
    
    int id = db_get(filepath, buffer);
    if (id > -1) {
        printf("%s has an ID of %d\n", buffer, id);
    }
    else {
        printf("%s was not found in the database.\n", buffer);
    }
}

void cmd_remove(char *filepath) {
    char buffer[100];
    printf("Enter the name to remove\n> ");
    fgets(buffer, 100, stdin);
    
    unsigned long end = strlen(buffer) - 1;
    if (buffer[end] == '\n') buffer[end] = '\0';
    
    db_remove(filepath, buffer);
}

void cmd_print(char *filepath) {
    db_print(filepath);
}

void print_usage() {
    printf("USAGE:\n");
    printf("\tfladb (-i | -d) [path]\n");
    printf("\tNote: path not needed if -i option specified.\n");
}

void run_interactive() {
    printf("Welcome to FlatDB.\n");
    printf("Copyright 2013 Oracle Corporation\n");
    printf("\"We synergize your paradigms.\"\n");
    printf("\n");
    
    char path[200];
    printf("Enter the location of the database file. It does not need to exist; a blank one will be created for you if necessary.\n> ");
    fgets(path, 200, stdin);
    
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
                cmd_add(path);
                break;
                
            case 'f':
                cmd_get(path);
                break;
                
            case 'r':
                cmd_remove(path);
                break;
                
            case 'p':
                cmd_print(path);
                break;
                
            default:
                break;
        }
    } while (command != 'q');
    
    printf("Thank you. Goodbye.\n");
}

void run_multiprocess(int argc, const char **argv) {
    char **args;
    args = (char**)malloc(sizeof(char*)*4);
    
    args[0] = argv[0];
    args[1] = "-w";
    args[2] = argv[2];
    args[3] = (char *)0;
    
    int child1, child2, child3;
    if ((child1 = fork()) == 0) {
        printf("%d: Gonna execute.\n", getpid());
        execv(*args, args);
    }
    else if ((child2 = fork()) == 0) {
        printf("%d: Gonna execute.\n", getpid());
        execv(*args, args);
    }
    else if ((child3 = fork()) == 0) {
        printf("%d: Gonna execute.\n", getpid());
        execv(*args, args);
    }
    else {
        sleep(1000);
        int status;

        int pid;
        
        int procs[3];
        procs[0] = child1;
        procs[1] = child2;
        procs[2] = child3;
        
        while (1) {
            pid = wait(&status);
            printf("Checking child 1\n");
            if (pid == child1) {
                printf("Child 1 done.\n");
                procs[0] = 0;
            }
            printf("Checking child 2\n");
            if (pid == child2) {
                printf("Child 2 done.\n");
                procs[1] = 0;
            }
            printf("Checking child 3\n");
            if (pid == child3) {
                printf("Child 3 done.\n");
                procs[2] = 0;
            }
            if (procs[0] == 0 && procs[1] == 0 && procs[2] == 0) {
                break;
            }
        }
        
        cmd_print(argv[2]);
    }
}

void run_worker(int argc, const char **argv) {
    char lockfile[200];
    strcpy(lockfile, argv[2]);
    strcat(lockfile, ".lock");
    int fd;
    int pid = getpid();
    
    for (int i = 0; i < 10; i++) {
        printf("%d: Going to add\n", pid);
        while ((fd = open(lockfile, O_CREAT | O_EXCL)) < 3) {
            printf("%d: File was locked.\n", pid);
            sleep(100);
        }
        Person to_add;
        to_add.id = pid;
        sprintf(to_add.name, "process_%d", to_add.id);
        
        db_add(argv[2], &to_add);
        close(fd);
        unlink(lockfile);
    }
    
    for (int i = 0; i < 9; i++) {
        printf("%d: Going to remove\n", pid);
        while ((fd = open(lockfile, O_CREAT | O_EXCL)) < 3) {
            printf("%d: File was locked.\n", pid);
            sleep(100);
        }
        char name[30];
        sprintf(name, "process_%d", pid);
        db_remove(argv[2], name);
        close(fd);
        unlink(lockfile);
    }
}



