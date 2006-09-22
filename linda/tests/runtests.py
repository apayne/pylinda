#!/usr/bin/python

import unittest

import server

alltests = unittest.TestSuite((server.suite(), ))

if __name__ == "__main__":
    runner = unittest.TextTestRunner()
    runner.run(alltests)
