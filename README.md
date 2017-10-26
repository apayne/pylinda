# pylinda
(Original: code.google.com/p/pylinda) A Linda implementation in Python. 

Linda provides extensions that allow you to divide work between multiple processes, possibly on different machines.  It uses the concept of a "tuplespace" which stores various amounts of unstructured data.  Client processes can then access this tuplespace, extract a tuple for processing, process it, and even place a new tuple back into tuplespace.
