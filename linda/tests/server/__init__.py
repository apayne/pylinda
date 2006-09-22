import unittest

import tuplecontainer

def suite():
    return unittest.TestSuite((tuplecontainer.suite(), ))

