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

## \namespace tuplecontainer
## \brief This module contains the class the contains tuples
## \internal
##
## \author Andrew Wilkinson <aw@cs.york.ac.uk>
##

from match import compare

def doesMatch(e1, e2):
    if isinstance(e1, tuple):
        if len(e1) != len(e2):
            return False
        for t1, t2 in zip(e1, e2):
            if not doesMatch(t1, t2):
                return False
        return True
    elif e1.isType():
        if compare(e1, e2.type):
            return True
    else:
        if compare(e1.type, e2.type) and e1 == e2:
            return True

class TupleContainer:
    def __init__(self):
        self.contain = []

    def add(self, tup):
        self.contain.append(tup)

    def matchOneTuple(self, template):
        return self.matchTuples(template).next()

    def matchTuples(self, template):
        for t in self.contain:
            if len(template) != len(t):
                continue
            #print "match", t, "against", template
            sucess = 0
            for e1, e2 in zip(template, t):
                if doesMatch(e1, e2):
                    sucess += 1
                else:
                    break
            if sucess == len(template):
            #    print "yes"
                yield t
            #else:
            #    print "no"

    def matchAllTuples(self):
        return self.contain[:]

    def delete(self, tup):
        for i in range(len(self.contain)):
            t = self.contain[i]
            if len(tup) != len(t):
                continue
            sucess = 0
            for e1, e2 in zip(tup, t):
                if doesMatch(e1, e2):
                    sucess += 1
                else:
                    break
            if sucess == len(tup):
                del self.contain[i]
                return

    def isEmpty(self):
        return len(self.contain) == 0

    def count(self):
        return len(self.contain)


#
# Tuplecontainer uses a trie structure to provide an efficiant method of storing tuples
# see http://en.wikipedia.org/wiki/Trie
#
#class TupleContainer:
    #def __init__(self):
        ##
        ## self.contain is a dictionary, the keys of which are the elements of the tuples
        ## The values are a pair, a nubmer representing the number of tuples than end there, and another
        ## TupleContainer which contains tuples with more elements
        ##
        #self.contain = {}
        #self.empty = 0

    #def add(self, tup):
        #if len(tup) == 0:
            #self.empty += 1
        #elif len(tup) == 1:
            ## If we're entering the last element of a tuple either increment the count if we've seen
            ## this tuple before, otherwise create a new entry in the dictionary
            #if self.contain.has_key(tup[0]):
                #self.contain[tup[0]][0] += 1
            #else:
                #self.contain[tup[0]] = [1, None]
        #else:
            ## If we have a key for this element of the tuple then add the rest of the tuple to 
            ## appropriate sub-trie. If not create it, then add it.
            #if self.contain.has_key(tup[0]):
                #if self.contain[tup[0]][1] is None:
                    #self.contain[tup[0]][1] = TupleContainer()
                #self.contain[tup[0]][1].add(tup[1:])
            #else:
                #self.contain[tup[0]] = [0, TupleContainer()]
                #self.contain[tup[0]][1].add(tup[1:])

    #def matchOneTuple(self, template):
        ## Just return the first tuple
        #try:
            #return self.matchTuples(template).next()
        #except NoTuple:
            #raise NoTuple, template

    #def matchTuples(self, template):
        #if len(template) == 0:
            #for i in range(self.empty):
                #yield ()
            #raise NoTuple
        ## take the first element in, and then the rest of, the tuple
        #ele, template = template[0], template[1:]
        ## take a list of all the tuple elements we contain
        #tups_list = self.contain.keys()

        #if template == ():
            ## if the rest of the template is empty then this is the last element to match
            #try:
                #self.contain[ele]
            #except KeyError:
                ## we don't contain the element - but maybe we can still match it
                #if type(ele) == tuple:
                    #for t in tups_list:
                        ## if the sub element is a tuple then match that...
                        #if doesMatch(ele, t):
                            #for i in range(self.contain[t][0]):
                                #yield (t, )
                #else:
                    #for t in tups_list:
                        #if (ele == t.__class__) or (ele == t):
                            ## is the element we're looking for the base class of t, or is t then we've 
                            ## found it
                            #for i in range(self.contain[t][0]):
                                #yield (t, )
            #else:
                ## we contain this tuple, so yield the same number of times as it is stored
                #for i in range(self.contain[ele][0]):
                    #yield (ele, )
        #else:
            ## we're not looking for the last element in this tuple...
            #try:
                ## do we have an entry for the element in the template
                #self.contain[ele]
            #except KeyError:
                ## no we don't... look through each element and see if it matches the template
                #if type(ele) == tuple:
                    #for t in tups_list:
                        #if doesMatch(ele, t) and self.contain[t][1] is not None:
                            #m = self.contain[t][1].matchTuples(template)
                            #while True:
                                #try:
                                    #yield (t,) + m.next()
                                #except NoTuple:
                                    #break
                #else:
                    #for t in tups_list:
                        #if (ele == t.__class__) or (ele == t):
                            #if self.contain[t][1] is not None:
                                #m = self.contain[t][1].matchTuples(template)
                                #while True:
                                    #try:
                                        #yield (t,) + m.next()
                                    #except NoTuple:
                                        #break
            #else:
                ## we've got a match - as we have more elements to match keep looking...
                #if self.contain[ele][1] is not None:
                    #m = self.contain[ele][1].matchTuples(template)
                    #while True:
                        #try:
                            #yield (ele, ) + m.next()
                        #except NoTuple:
                            #break
        #raise NoTuple

    #def matchAllTuples(self):
        ## take a list of all the tuple elements we contain
        #tups_list = self.contain.keys()

        #for tup in tups_list:
            #for i in range(self.contain[tup][0]):
                #yield (tup, )
            #if self.contain[tup][1] is not None:
                #for tuple in self.contain[tup][1].matchAllTuples():
                    #yield (tup, ) + tuple

    #def delete(self, tup):
        #try:
            #if len(tup) == 0:
                #self.empty -= 1
                #return
            #elif len(tup) == 1:
                ## this is the last element in the tuple to delete, so just decrement the count
                #assert self.contain[tup[0]][0] > 0
                #self.contain[tup[0]][0] -= 1
            #else:
                ## there is more, so just continue deleting.
                #self.contain[tup[0]][1].delete(tup[1:])
            #if self.contain[tup[0]][0] == 0 and (self.contain[tup[0]][1] is None
                                             #or self.contain[tup[0]][1].isEmpty()):
                ## check to see if we can delete the sub-trie to save space
                #del self.contain[tup[0]]
        #except KeyError:
            #raise SystemError, "Error deleting tuple %s, tuple does not exist" % (str(tup))

    #def isEmpty(self):
        ## if we don't contain any elements then we're empty.
        #return len(self.contain) + self.empty == 0

    #def count(self):
        #i = self.empty
        #for e in self.contain:
            #i = i + self.contain[e][0]
            #if self.contain[e][1] is not None:
                #i = i + self.contain[e][1].count()
        #return i
