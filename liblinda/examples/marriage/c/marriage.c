#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "linda.h"

#define count 20
char* men[] = {"AIDAN", "JADEN", "CADEN", "ETHAN", "CALEB", "DYLAN", "JACOB", "JORDAN", "LOGAN", "HAYDEN", "CONNOR", "RYAN", "MORGAN", "CAMERON", "ANDREW", "JOSHUA", "NOAH", "MATTHEW", "ADDISON", "ASHTON"};
char* women[] = {"MADISON", "EMMA", "ABIGAIL", "RILEY", "CHLOE", "HANNAH", "ALEXIS", "ISABELLA", "MACKENZIE", "TAYLOR", "OLIVIA", "HAILEY", "PAIGE", "EMILY", "GRACE", "AVA", "AALIYAH", "ALYSSA", "FAITH", "BRIANNA"};

void makeOrderList(int len, int* list);

void Man(int id) {
    int order[count];

    printf("Man %i.\n", id);

    makeOrderList(count, order);

    if(!Linda_connect(Linda_port)) {
        fprintf(stderr, "Unable to connect to Linda server.\n");
    }

    Linda_disconnect();
}

void Woman(int id) {
    int order[count];

    printf("Woman %i.\n", id);

    makeOrderList(count, order);

    if(!Linda_connect(Linda_port)) {
        fprintf(stderr, "Unable to connect to Linda server.\n");
    }

    Linda_disconnect();
}

int main(int argc, char* argv[]) {
    int i;

    char* num = malloc(10);
    if(argc < 2) {
        printf("Invalid command line arguments.\n", argv[1]);
    } else if(strcmp(argv[1],"all") == 0) {
        for(i=0; i<count; i++) {
            sprintf(num, "%i", i);
            if(fork() == 0) {
                execl(argv[0], argv[0], "man", num, NULL);
            }
        }
        for(i=0; i<count; i++) {
            sprintf(num, "%i", i);
            if(fork() == 0) {
                execl(argv[0], argv[0], "woman", num, NULL);
            }
        }
    } else if(strcmp(argv[1],"men") == 0) {
        for(i=0; i<count; i++) {
            sprintf(num, "%i", i);
            if(fork() == 0) {
                execl(argv[0], argv[0], "man", num, NULL);
            }
        }
    } else if(strcmp(argv[1], "women") == 0) {
        for(i=0; i<count; i++) {
            sprintf(num, "%i", i);
            if(fork() == 0) {
                execl(argv[0], argv[0], "woman", num, NULL);
            }
        }
    } else if(strcmp(argv[1], "man") == 0) {
        Man(atoi(argv[2]));
    } else if(strcmp(argv[1], "woman") == 0) {
        Woman(atoi(argv[2]));
    } else {
        printf("%s. Invalid command line arguments.\n", argv[1]);
    }
    free(num);
}

void makeOrderList(int len, int* list) {
    int i;
    int tmp;
    for(i=0; i<len*len; i++) {
        int a = (int)(rand() % len);
        int b = (int)(rand() % len);
        tmp = list[a];
        list[a] = list[b];
        list[b] = tmp;
    }
    for(i=0; i<count; i++) {
        list[i] = i;
    }
    for(i=0; i<count*count; i++) {
        int a = rand() % count;
        int b = rand() % count;
        int tmp = list[a];
        list[a] = list[b];
        list[b] = tmp;
    }
}
