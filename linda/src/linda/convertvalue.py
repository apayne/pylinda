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

import _linda_server

def convertValue(code, val):
    regs = [val]
    print "convert", val
    for c in code:
        if c[0] == "SETTYPEID":
            regs[-1].type_id = c[1]
        elif c[0] == "EVAL":
            regs[-1] = convertValue(c[1], regs[-1])
        elif c[0] == "EXPLODE":
            v = regs.pop()
            regs.extend(explode([v])[::-1])
        elif c[0] == "ROTATE":
            v = regs.pop()
            regs = [v] + regs
        elif c[0] == "CONSTRUCT":
            regs = [construct(regs[::-1], c[1])]
        elif c[0] == "SUMCASE":
            regs[-1] = convertValue(c[1][regs[-1].sum_pos], regs[-1].value)
        elif c[0] == "CREATESUM":
            regs[-1] = _linda_server.Sum(regs[-1], c[1])
        else:
            raise SystemError, c[0]
    print "got", regs[-1]
    return regs[-1]

def explode(reg):
    r = []
    for e in reg:
        if e.isTuple():
            r.extend(explode(list(e)))
        else:
            r.append(e)
    return r

def construct(reg, type):
    if type.isProductType():
        v = []
        for i in range(len(type)):
            v.append(construct(reg, type[i]))
        return _linda_server.Value(tuple(v))
    elif type.isId() and type.id not in builtin:
        t = lookupType(type.id_type_id)
        if t.isProductType():
            return construct(reg, t)
        else:
            v = reg[0]
            del reg[0]
            return v
    else:
        v = reg[0]
        del reg[0]
        return v

from match import builtin

if _linda_server.use_types:
    from type_cache import lookupType
