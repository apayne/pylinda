/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of LibLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    LibLinda is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    LibLinda is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with LibLinda; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "linda_internal.h"

#ifdef USE_DOMAIN_SOCKETS
#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_key_t key = 0;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void Linda_threading_start();
void Linda_thread_exit();

Linda_thread_data* Linda_get_thread_data() {
    Linda_thread_data* ptr;

    pthread_once(&key_once, Linda_threading_start);

    ptr = (Linda_thread_data*)pthread_getspecific(key);
    if(ptr == NULL) {
        ptr = (Linda_thread_data*)malloc(sizeof(Linda_thread_data));
        pthread_setspecific(key, ptr);

        ptr->sd = 0;
        ptr->thread_id = NULL;
    }

    return ptr;
}

static void Linda_threading_start() {
    pthread_key_create(&key, &Linda_thread_exit);
}

void Linda_thread_exit(void* tdata_ptr) {
    Linda_thread_data* tdata = (Linda_thread_data*)tdata_ptr;
    if(tdata->sd != 0) {
        shutdown(tdata->sd, SHUT_RDWR);
        tdata->sd = 0;
    }
    if(tdata->thread_id) {
        free(tdata->thread_id);
        tdata->thread_id = NULL;
    }
}
