/**

\page tutorial Tutorial

This is a brief tutorial on how to write a program to use the PyLinda system.

\section connect Connecting

The first thing any linda client will want to do is to connect to the linda network. This is done by importing the linda package and then calling the connect function.

\code
import linda
linda.connect()
\endcode

\section create Creating a TupleSpace

All linda clients have access to one tuplespace, the universal tuplespace, however it is generally used merely as a way for various process to contact each other. The majority of communication should be done through other tuplespaces. You can create new tuplespace by creating an instance of the \link linda::TupleSpace TupleSpace \endlink class.

\code
ts = linda.TupleSpace()
\endcode

Your master process will usually want to output the tuplespace reference to the universal tuplespace where the slave processes can read it.

The master process...
\code
linda.universe._out(("My identifying string", ts))
\endcode

The slave process...
\code
ts = linda.universe._rd(("My identifying string", linda.TupleSpace))[1]
\endcode

\section ops Operations on a TupleSpace

Now you have access to a private tuplespace you'll want to perform some operations on it. All the available operations can be found \link linda::TupleSpace here \endlink

*/
