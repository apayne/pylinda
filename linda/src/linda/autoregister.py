#    Copyright 2004-2006 Andrew Wilkinson <aw@cs.york.ac.uk>
#
#    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
#
#    PyLinda is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2.1 of the License, or
#    (at your option) any later version.
#
#    PyLinda is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with PyLinda; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import copy

import _linda_server

def convertFrom_none(x): return x
def convertTo_none(x): return x

def convertFrom_unregistered(value):
    if isinstance(value, tuple):
        return tuple([convertFrom_unregistered(v) for v in value])
    elif isinstance(value, _linda_server.Value):
        if False:
            pass
        else:
            raise SystemError, (value, type(value))
    else:
        return value

def convertTo_unregistered(value):
    if isinstance(value, tuple):
        value = copy.deepcopy(value)
        return tuple([convertTo_unregistered(v) for v in value])
    elif isinstance(value, _linda_server.Value):
        if False:
            pass
        else:
            raise SystemError, (value, type(value))
    else:
        return value

if not _linda_server.use_types:
    convertFrom = convertFrom_none
    convertTo = convertTo_none
elif _linda_server.register_types:
    convertFrom = convertFrom_none
    convertTo = convertTo_none
else:
    convertFrom = convertFrom_unregistered
    convertTo = convertTo_unregistered

from connections import Connection
