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

#include "flatdb.h"

int main(int argc, const char * argv[])
{
    Person *people = (Person *)malloc(sizeof(Person)*5);
    people[0].id = 1;
    people[0].name = "Bill Gates";
    
    people[1].id = 2;
    people[1].name = "Steve Jobs";
    
    people[2].id = 3;
    people[2].name = "Richard Stallman";
    
    people[3].id = 4;
    people[3].name = "Dennis Ritchie";
    
    people[4].id = 5;
    people[4].name = "Balls Shepard";
    
    int fd = open("/Users/lw322/Documents/db.txt", O_RDWR | O_CREAT | O_APPEND);
    
    if (fd > 2) {
        db_add(fd, &people[0]);
        db_add(fd, &people[1]);
        db_add(fd, &people[2]);
        db_add(fd, &people[3]);
        db_add(fd, &people[4]);
    }
    
    close(fd);
    
    
    return 0;
}



