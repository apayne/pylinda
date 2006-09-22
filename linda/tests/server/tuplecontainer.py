import unittest

import random

from linda.tuplecontainer import TupleContainer

class TupleContainerTestCase(unittest.TestCase):
    def setUp(self):
        self.ts = TupleContainer()
    def tearDown(self):
        self.ts = None

    def testDefaultSize(self):
        assert self.ts.count() == 0, 'Default size not zero'
        assert self.ts.isEmpty(), 'Default size not empty'
    def testAddEmpty(self):
        self.ts.add(())
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddOne(self):
        self.ts.add((1,))
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddTwo(self):
        self.ts.add((1,1))
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddThree(self):
        self.ts.add((1,1,1))
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddMany(self):
        for i in range(1000):
            self.ts.add((random.randint(0, 10),random.randint(0, 10),random.randint(0, 10)))
        assert self.ts.count() == 1000, 'Size not 1000 after 1000 adds (%i)' % (self.ts.count(), )
    def testAddString(self):
        self.ts.add(("abc", "def"))
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddInt(self):
        self.ts.add((1, 2))
        assert self.ts.count() == 1, 'Size not one after add'
    def testAddFloat(self):
        self.ts.add((1.0, 2.0))
        assert self.ts.count() == 1, 'Size not one after add'

def suite():
    suite = unittest.TestSuite()
    suite.addTest(TupleContainerTestCase("testDefaultSize"))
    suite.addTest(TupleContainerTestCase("testAddEmpty"))
    suite.addTest(TupleContainerTestCase("testAddOne"))
    suite.addTest(TupleContainerTestCase("testAddTwo"))
    suite.addTest(TupleContainerTestCase("testAddThree"))
    suite.addTest(TupleContainerTestCase("testAddMany"))
    suite.addTest(TupleContainerTestCase("testAddString"))
    suite.addTest(TupleContainerTestCase("testAddInt"))
    suite.addTest(TupleContainerTestCase("testAddFloat"))
    return suite