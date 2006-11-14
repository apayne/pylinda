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

builtin = ["bool", "int", "float", "string"]

def compare(t1, t2):
    assert t1.isType()
    assert t2.isType()

    if t1.isNil() and t2.isNil():
        return True
    elif t1.isId() and t2.isId():
        return t1.id in builtin and t1.id == t2.id
    elif t1.isProductType() and t2.isProductType():
        raise NotImplementedError
    elif t1.isSumType() and t2.isSumType():
        raise NotImplementedError
    elif t1.isPtr() and t2.isPtr():
        raise NotImplementedError
    elif t1.isFunction() and t2.isFunction():
        raise NotImplementedError
    else:
        return False
