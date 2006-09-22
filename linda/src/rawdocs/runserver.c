/**

\page runserver Running The Server

Each computer in your linda network is required to have a server running on it. The server coordinates access to any tuplespaces it stores and controls any client proccesses running locally to it.

The simplest way to run a server is simply to execute the following command...
\code
linda_server.py
\endcode

This will execute a single server, which will become a 'master' server. The only difference between a master server and an ordinary server is that the master server is the physical store of the universal tuplespace. This is a purely implementational detail as any client process need not know anything about the location of any tuplespaces.

In order to add a new server to the linda network run the server with the <b>-c</b> option followed by an ip address or a dns name of a computer already connection to your linda network.

\code
linda_server.py -cvenice
linda_server.py --connect=venice
\endcode

It is only necessary to specify the name/address of one computer as the server will automatically detect and connect to other servers as necessary.

Should the default port of 2102 not be suitable for some reason it is possible to change the port that the server listens on.

\code
linda_server.py -s2103
linda_server.py --server-port=2103
\endcode

\code
linda_server.py -cvenice -p2103
linda_server.py -cvenice --connect-port=2103
\endcode

The first set of commands show how to run the server on a different port, the following two show how to connect to a server running on a different port. These two options can naturally be combined.

If the server is running on a different port then any client programs will need to connect on that port - this can be done by passing the new port to the <b>connect</b> function.

\code
linda.connect(2103)
\endcode

See the \link marriage.py marriage.py \endlink example for a concrete example.

The another server option prevents any program running not running locally from connecting. While preventing any use as a distributed computing platform, if want to prevent any security problems<sup>*</sup> from running PyLinda then this option may come in handy.

\code
linda_server.py -l
linda_server.py --localbind
\endcode

 <sup>*</sup> Please note: this does not mean that using this option will make PyLinda completly secure, only that the chance of a security flaw being found is reduced.

On UNIX all communication between server and client programs is through unix domain sockets. This can be disabled by using the -d switch. TCP/IP sockets - this is slow and inefficient. By activating two server options Unix Domain Sockets and SystemV IPC can be used instead, these are much quicker however they are still experimental and have a habit of not cleaning up correctly when a client program crashes.

\code
linda_server.py -m -d
\endcode

*/
