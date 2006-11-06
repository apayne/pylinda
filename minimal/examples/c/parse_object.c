/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of Minimal (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    Minimal is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    Minimal is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Minimal; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "minimal.h"

int main(int argc, char* argv[]) {
    char* buf = malloc(1); buf[0] = '\0';
    char tmp[1024];
    int i;

    Minimal_init();

    i = 1024;
    while(i == 1024) {
        char* buf2;
        i = fread(tmp, 1, 1024, stdin); tmp[i] = '\0';
        buf2 = (char*)malloc(strlen(buf)+i+1);
        strcpy(buf2, buf);
        strcpy(&(buf2[strlen(buf)]), tmp);
        free(buf);
        buf = buf2;
    }

    MinimalValue value = Minimal_parseValue(buf);
    char* s = Minimal_Value_string(value);

    printf("%s\n", s);

    free(s);
    Minimal_delReference(value);
    free(buf);

    Minimal_finalise();

    return 0;
}
