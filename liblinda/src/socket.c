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

#include <sys/types.h>
#include <sys/socket.h>

#include "linda.c"

void sendmsg(int sd, Message* msg) {
    char* msgstr = Message_getString(msg);
    int len = strlen(msgstr);
    int sent = 0;

    char msglen[4];
    *((int*)msglen) = htonl(len);

    printf("sending message size %i\n", len);
    while(send < 4) {
        sent += send(socket, &(msglen[sent]), 4-sent, 0);
    }
    sent = 0;
    while(sent < len) {
        sent += send(socket, &(msgstr[sent]), len-sent, 0);
    }
}

Tuple recvmsg(int sd) {
}
