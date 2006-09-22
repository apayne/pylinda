/**

\page beginner A Beginners Guide To Linda

First a server must be started - \c linda_server.py

Then a client program must be started, the simplest is just the python interactive interpreter.

\code
bash$ python
>>> import linda
>>> linda.connect()
>>> linda.universe._out((1,2,3))
\endcode 

Now quit that interpreter, and start a new one...

\code
bash$ python
>>> import linda
>>> linda.connect()
>>> linda.universe._in((int, int, int))
(1, 2, 3)
\endcode

If you want to add a new computer to the linda network simply run 'linda_server.p -c&lt;ip address or dns name&gt;' where the computer you supply is already running a linda server. 

*/