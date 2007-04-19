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

#ifndef __LINDA_SERVER_H__
#define __LINDA_SERVER_H__

#define LINDA_SERVER
#include "linda_internal.h"

LSEXPORT unsigned char Linda_serve(unsigned char use_domain, int port);
LSEXPORT int Linda_accept(int sd);
LSEXPORT int Linda_server_disconnect();

LSEXPORT int Linda_connect(char* address);
LSEXPORT void Linda_disconnect(int sd);
LSEXPORT void Linda_finalise();

LSIMPORT extern int Linda_port;

LSEXPORT void Linda_setNodeID(char* nid);

#endif
