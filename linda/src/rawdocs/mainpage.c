/**
\mainpage PyLinda V0.6

\section Intro Introduction

Linda is an widely studied distributed computing environment, centered around
the notion of a tuple space. A tuple space is a bag (also called a multi-set) of
tuples. A tuple is an ordered, typed chunk of data. Tuple spaces exist
independently of processes in the system, and the data placed into a tuple space
also exist independently. See "Generative communication in Linda" (1985) and
"Multiple tuple spaces in Linda" both by David Gelernter for more information on
Linda.

PyLinda is a simple implementation of a linda system, however it also includes
several of the more recently proposed extensions to Linda in the form of
multiple tuple spaces, garbage collection, sane non-blocking primitives and bulk
tuple operations.

For installation instructions see the \link install "How To Install" page\endlink.

\section Howto A Brief How To

Firstly you need to create a network of one or more Linda servers. This can be done by executng the following command
on one node of the network.
\code
linda_server.py
\endcode

and then executing
\code
linda_server.py -c<ip address or dns name>
\endcode

on each of the other nodes where the ip address or dns name is for a node where the server has already been started. For a more detailed view on how to run the server see \link runserver "Running The Server" \endlink.

On any of the computers that have a server started you can then start Python.

\code
>>> import linda
>>> linda.connect()
>>> linda.universe._out((1, 2, 3))
>>> linda.universe._in((1, 2, int))
(1, 2, 3)
\endcode

\note For details on how to write a program to take advantage of the linda system see the \link tutorial tutorial\endlink page.

\note For more detailed examples look at the examples section.

*/
