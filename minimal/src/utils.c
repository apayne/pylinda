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

#include <string.h>

#include "minimal_internal.h"

unsigned char Minimal_isBuiltIn(char* type_name) {
    if(strcmp(type_name, "int8") == 0 || 
       strcmp(type_name, "int16") == 0 || 
       strcmp(type_name, "int32") == 0 || 
       strcmp(type_name, "int64") == 0 || 
       strcmp(type_name, "uint8") == 0 || 
       strcmp(type_name, "uint16") == 0 || 
       strcmp(type_name, "uint32") == 0 || 
       strcmp(type_name, "uint64") == 0 || 
       strcmp(type_name, "ieeesingle") == 0 || 
       strcmp(type_name, "ieeedouble") == 0 || 
       strcmp(type_name, "bool") == 0 || 
       strcmp(type_name, "asciichar") == 0 || 
       strcmp(type_name, "unicodechar") == 0 || 
       strcmp(type_name, "string") == 0 || 
       strcmp(type_name, "type") == 0 || 
       strcmp(type_name, "tuplespace") == 0
    ) {
        return 1;
    } else {
        return 0;
    }
}
