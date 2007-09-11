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

def wrap_function(arg_type, arg, result_type, result, func):
    print "func:", func

    print func.func_name

    code = """
wrappedfunction :: %s -> %s;
wrappedfunction x = let arg=%s let val=%s(arg) (%s);
""" % (stringify_type(arg_type), stringify_type(result_type), wrap("x", arg), func.func_name, wrap("val", result))

    print code

    return _linda_server.Function(code)

def wrap(argname, arg, vars=None):
    if vars is None:
        vars = []

    for c in arg:
        print c, vars

        if c[0] == "EXPLODE":
            vars.extend(explode(argname, c[1]))
        elif c[0] == "CONSTRUCT":
            vars.append(construct(c[1], vars))
        elif c[0] == "EVAL":
            vars[-1] = wrap(argname, c[1], [vars[-1]])
        elif c[0] == "SETTYPEID":
            if c[1] is not None:
                vars[-1] = "settypeid(%s, \"%s\")" % (vars[-1], c[1])
        elif c[0] == "ROTATE":
            v = vars.pop()
            vars = [v] + vars
        else:
            raise SystemError

    return vars[-1]

def explode(argname, type, keys=None):
    if keys is None:
        keys = []
    if not type.isProductType():
        return [argname + "".join(["[%i]" % (i, ) for i in keys])]
    try:
        vars = []
        keys.append(0)
        for i in range(len(type)):
            vars.extend(explode(argname, type[i], keys))
            keys[-1] = keys[-1] + 1
        return vars
    finally:
        keys.pop()

def construct(type, vars):
    if not type.isProductType():
        return vars
    r = []
    for i in range(len(type)):
        print i, vars, type[i]
        if len(vars) > 0:
            r.append(construct(type[i], vars.pop()))
        else:
            r.append("Nil")
    return "(" + ", ".join(r) + ")"

def stringify_type(arg):
    if arg.isNil():
        return "Nil"
    elif arg.isId():
        return arg.id
    elif arg.isProductType():
        if len(arg) == 0:
            return "()"
        elif len(arg) == 1:
            return "(" + stringify_type(arg[0]) + " * 1)"
        else:
            return "(" + " * ".join([stringify_type(x) for x in arg]) + ")"
    elif t1.isSumType():
        return "(" + " + ".join([stringify_type(x) for x in arg]) + ")"
    elif arg.isFunctionType():
        return "(" + stringify_type(arg.param) + " -> " + stringify_type(arg.result) + ")"
    else:
        raise SystemError, arg

def __getVariableName():
    i = 0
    while True:
        yield "x" + str(i)
        i =+ 1
getVariableName = __getVariableName()
