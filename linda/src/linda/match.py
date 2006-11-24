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

def compare(t1, t2, checked=None):
    assert t1.isType()
    assert t2.isType()

    if checked is None:
        checked = [(t1, t2)]
    elif (t1, t2) in checked:
        return True
    else:
        checked.append((t1, t2))

    try:
        if t1.isNil() and t2.isNil():
            return True
        elif t1.isId() and t2.isId():
            if t1.id in builtin or t2.id in builtin:
                return t1.id == t2.id
            else:
                return compare(t1.typemap[t1.id], t2.typemap[t2.id], checked)
        elif t1.isProductType() and t2.isProductType():
            if len(t1) != len(t2):
                return False
            for i in range(len(t1)):
                e1, e2 = t1[i], t2[i]
                if not compare(e1, e2, checked):
                    return False
            return True
        elif t1.isSumType() and t2.isSumType():
            if len(t1) != len(t2):
                return False
            for i in range(len(t1)):
                e1, e2 = t1[i], t2[i]
                if not compare(e1, e2, checked):
                    return False
            return True
        elif t1.isPtrType() and t2.isPtrType():
            return compare(t1.ptrtype, t2.ptrtype, checked)
        elif t1.isFunctionType() and t2.isFunctionType():
            return compare(t1.arg, t2.arg, checked) and compare(t1.result, t2.result, checked)
        else:
            return False
    finally:
        checked.pop()
