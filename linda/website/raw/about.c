/**

\page about About Linda

Linda is a very abstract coordination system which provides a simple interface for communication between processes. The central notion in Linda is the tuplespace. It is through tuplespaces that all communication is done. Processes do not need to know anything about other processes that are running in the system, they simply put messages into a tuplespace or wait for an appropriate message to appear.

\section Tuplespaces Tuplespaces

A tuplespace is bag of tuples, used as the sole communication medium in <i>Linda</i>-like systems. A tuplespace is a virtually shared associative memory structure in that it is accessible to all processes, no matter what their physical location. It also makes no formal requirement of the actual distributive nature of the implementation. All tuples in a tuplespace may be stored on a single node of the network, or they may be spread out over several nodes. Providing the tuplespace can be accessed from any node on the network it doesn't matter where the tuples are stored. A tuplespace is an associative structure because tuples are accessed based on their value and structure, not by address.

As described in [1] tuplespaces are a generative communication method because when a tuple is added to a tuplespace by a process, <i>A</i>, then even if that process A dies the tuple remains in the tuplespace until it is requested by another process.

The main elements of a tuplespace based system are...

<ul>
<li><b>Tuplespace</b> This is a shared memory structure used to hold tuples. The early tuplespace based systems had only one tuplespace and allowed three operations on it - outputting a tuple, destructively inputting a tuple, and nondestructively reading a tuple.</li>

<li><b>Tuples</b> A tuple is an ordered collection of values. The tuple <tt>&lt; 1, ``Bob" &gt;</tt> is a tuple of size 2, with an integer as the first element, and a string as the second.</li>

<li> <b>Template</b> A template is a tuple, but with some or all elements replaced by a placeholder that can be matched against tuples contained in the tuplespace. For example the template <tt>&lt; 1, string &gt;</tt> will match <tt>&lt; 1, {}``Bob" &gt;</tt>, but not <tt>&lt; 2, {}``Bob" &gt;</tt>. In also will not match <tt>&lt; 1, 2 &gt;</tt> as the second element is not a string.</li>
</ul>

<i>Linda</i> has three primitive operations which are described below. Other tuplespace based systems have similar, if not identical, primitives.

<ul>
<li><tt>out(t<sub>1</sub>)</tt> Takes the tuple <tt>t<sub>1</sub></tt> and adds it to the tuplespace.</li>

<li><tt>in(template)</tt> Returns a tuple matching the template. The matched tuple is removed from the tuplespace. If no tuples matching the template are currently in the tuplespace then the call blocks until a tuple is outputted that does match. If there is more than one matching tuple it is unspecified which tuple will be returned. If more than one process calls <i>in</i> with the same template then it is unspecified which process will get the matching tuple.</li>

<li><tt>rd(template)</tt> Identical to <i>in</i> but creates a copy of the tuple and leaves the original in the tuplespace.</li>
</ul>

Given that Linda specifies that a tuplespace must be accessible from any location it is clear that it is distributed across space, as two processes running on separate computers can communicate easily. Linda is also independent of time as well since a process will block on at <i>in</i> or <i>rd</i> until such times an <i>out</i> is performed by another process, and any tuplespace is a persistent medium so a process can output a tuple in the knowledge that however long it is before another process tries to read it, the tuple will remain accessible.

There are fundamentally two different types of Linda system - open systems and closed systems. The difference is related to what programs are allowed to access the Linda network. The original implementation of Linda was a closed system, but the trend has been towards open systems as they provide much more flexibility, with the lose of some possibilities for optimisation. A closed system requires all programs to be compiled at the same time along with information about what processes will run where. This gives the compiler information that enables it to optimise the locations of tuples. It also has complete information about the types used.

An open implementation of Linda allows any program to connect to the Linda system, with no prior knowledge needed by the compiler. This obviously limits the type of optimisations that can be performed to those that are done dynamically. It also raises the possibility of programs with incompatible types trying to communicate. The benefits for using an open implementation are that the system is far more flexible in how it is compiled and set up.

\section Exten Extensions to Linda

Since the original specification for <i>Linda</i> was proposed a variety of implementations have been produced, each supporting a differing number of extensions that enhance the functionality of a basic <i>Linda</i> system.

\subsection Multiple Multiple Tuple Spaces

The initial specification of <i>Linda</i> had a single, global tuplespace - one of the most obvious extensions to this model is to allow multiple tuplespace. As [2, 3] discuss all that is needed is to add a new <i>Tuplespace</i> type (sometimes, as in <i>Isetl-Linda</i>[4], called <i>bag</i>), to modify the existing primitives to accept a <i>Tuplespace</i> parameter and a new <i>CreateTupleSpace</i> primitive.

<table border=0>
<tr><td>
<img src=nestedspaces.png>
</td></tr>
<tr><td>
<b>Figure 1:</b> Models of nested tuplespace.
</td></tr></table>

Initially multiple tuplespaces were implemented as a hierarchy[5] however the trend has been towards multiple disjoint tuplespace (as in Laura[6]). Figure 1 shows these two differing models graphically. This is probably because the flat model is more flexible; the added structure of the nested model is useful in some situation but counterproductive in others.

Rather than just having totally separate tuplespace another possibility is the use of scopes[7]. Intuitively a scope is an overlapping tuplespace that gives a view on some or all of a group of tuples. The ability to partition a tuplespace gives the ability to easily replicate the functionality of the <i>collect</i> and <i>copy-collect</i> primitives. Using scopes to partition a tuplespace improves on the <i>collect</i> primitive by moving tuples between different scopes of the same tuplespace, this allows the process to solve the multiple rd problem, and still allow other processes that have access to the whole tuplespace access to the tuples at all times.

\subsection NonBlock Non-Blocking Primitives

One of the features of the original Yale version of <i>Linda</i> was duplicates of the <i>in</i> and <i>rd</i> primitives, <i>inp</i> and <i>rdp</i>, that instead of blocking if no tuples match they returned a special value indicating no match. This has the significant problem of breaking the time independence of a <i>Linda</i> program. This results in a program being dependent on when it reaches each point in the program, because sometimes the call may return a tuple, other times it may not and there is no way to know what will happen. With the blocking versions the call will return only if there is a tuple that matches, otherwise it will never return.

It feels unnatural to write code that could block forever, however it is usually possible to rewrite the code in a form that will work with the blocking primitives, or the bulk tuple operations described in section <b>Bulk Tuple Operations</b>.

In [8], using logic similar to CSP[9], Jacob and Wood develop a semantic for the nonblocking primitives that maintains the time independent properties of <i>Linda</i>. An intuition for the semantic that they develop is that the operations do block until a clique of processes deadlocked on a tuplespace is detected. If this clique is made up of all processes who have knowledge of that tuplespace then then no progress can be made until one or more the processes is reactivated by making an <i>inp</i> call return without matching. This will require a kernel to maintain information about which processes know about which tuplespace and whether they are blocked on an <i>inp</i> or not - but most of this information will already be collected for the purpose of garbage collection and so is relatively straightforward to implement.

\subsection Bulk Bulk Tuple Operations

The classical <i>Linda</i> model suffers from a problem known as the <i>multiple rd problem</i>. If a process wishes to perform a non-destructive read on a subset of tuples then in the classic model all the tuples must be destructively read, processed and then returned to the tuplespace. This is clumsy and error prone especially if other processes are adding and removing tuples while this read is taking place.

The multiple read problem comes from the non-deterministic nature of Linda. The semantics of Linda do not specify which tuple you will receive if there is more than one tuple that matches the template being matched. They do not specify that you will receive a different tuple if you perform more than one read on an unchanged tuplespace. Indeed the usual implementation is to return the first matching tuple, if the tuplespace does not change then the same tuple will be returned each time. This makes it difficult to non-destructively determine how many matching tuples there are, unless you perform an inelegant hack as described above.

In [10, 11] a pair of new primitives are proposed <i>collect</i> and <i>copy-collect</i>.
Given two tuplespace <tt>t<sub>1</sub></tt> and <tt>t<sub>2</sub></tt> <tt>collect(t<sub>1</sub>,\: t<sub>2</sub>,\: template)</tt> moves all tuples that match <i>template</i> from <tt>t<sub>1</sub></tt> to <tt>t<sub>2</sub></tt>. <i>copy-collect</i> is similar except that it copies rather than moves the tuples between the two tuplespace. Both primitives return a integer representing the number of tuples that were affected by the operation.

A side benefit of using these new primitives is that the efficiency of the system is increased. If the operations are performed using repeated <i>in</i> or <i>rd</i> calls then many packets will be sent across the network. With a bulk tuple operation these packets can be condensed into (ideally) a single packet. [12] contains a quantitative demonstration of the speed advantage of sending multiple tuples per packet.

\subsection Implementation Linda Implementations

Linda has been shown in numerous papers to be a useful system from a research perspective, but if it doesn't translate well into a practical system then it would be worthless. There have been many implementations of Linda and Linda-like systems, each focusing of a different set of extensions the core system.

So far the semantics and overall structure of Linda have been discussed, however without an implementation all that is useless. Fortunately Linda has been implemented many times [4, 5, 6, 12, 2, 13, 14].

\subsection GC Garbage Collection

In an open implementation of Linda, with multiple tuplespaces, a major problem is memory usage. If a system implements the bulk tuple operations then a common design pattern is to create a new tuplespace, copy some tuples to it to count how many there are and then lose the handle to the tuplespace. If no garbage collection is implemented then this tuplespace will remain in existence indefinitely. It it obvious that this situation will quickly cause the system to run out of memory and fail. Also just as distributed systems support transparent, uniform placement of objects they should also support transparent object management including deletion of unreferenced objects.

Local garbage collection is a well understood problem, and has been implemented in many languages, such as Java and Python however expanding it to a distributed system is a challenge - tracing algorithms require
costly termination mechanisms while reference counting[15] is scuppered by common message failures. Most methods for performing distributed garbage collection only achieve some of the goals of such as system, namely efficiency, scalability and fault-tolerance.

In the 1995 survey paper[16] Plainefossé and Shapiro describe the various techniques one can use to perform garbage collection on a distributed system. The simplest method is to extend the classic reference counting garbage collection scheme. If a process creates a stub representing a remote object then it must send a message increasing the remote object's reference count, and similarly when the stub is destroyed. While this method is adequate in a theoretical system, it doesn't behave correctly in a system in which faults can occur and messages arrive out of order.

In his PhD thesis[17] Menezes describes a system to perform garbage collection in the <i>Linda</i>-like system, <i>Ligia</i>. He also deals with the related problem of implementing I/O operations in <i>Linda</i>. To ensure that garbage collection takes place correctly it is split into three distinct sections, firstly to ensure the system has the information required to perform garbage collection a checking in and out system for processes is required as well as tuple scanning - which detects when a reference to a tuplespace is transferred, secondly information is collected for garbage collection on each node of the system based on <i>local</i> tuplespace. Finally a global garbage collector is occasionally run to build a combined version of the local garbage graphs and determine any global tuplespaces that are garbage.

\section Conclusion Conclusion

Linda is a very abstract system with a very high level interface. This makes coordination between distributed processes very straightforward, and unlike most other systems is does not require all programs to be compiled together nor does it make any requirements about the languages used by the client programs.

Linda does have the disadvantage that matching based on types is not rigidly defined and consequently most, if not all, current implementations only allow a very basic types to be used in tuples.

[1] David Gelernter. Generative communication in Linda. ACM Trans Program. Lang Syst. 7(1):80 112, 1985.<br>
[2] David Gelerneter. Multiple tuple spaces in Linda. In E. Odijk, M. Rem and J. C Syre, editor, Parallel Architectures and Languages Europe (PARLE 89), volume 366 of LNCS, pages 20-27, Eindhoven, The Netherlands, 1989. Springer Verlag.<br>
[3] Susan Hupfer. Melinda: Linda with multiple tuple spaces. Master's thesis, Yale University Department of Computer Science, Feburary 1990, Research Report 766.<br>
[4] Antony Rowstron, Andrew Douglas, Alan Wood. ISETL-LINDA: Parallel Programming with Bags. Technical Report YCS-95-257. Department of Computer Science, University of York, York, England, YO10 5DD, 1995.<br>
[5] Nicholas Carriero, David Gelerner and Lenore D. Zuck. Bauhaus Linda. In Selected papers of Parallelism and Distribution, Object-Based Models and Languages for Concurrent System, pages 66-76. Springer-Verlag, 1995.<br>
[6] R. Tolksdort. Laura - A service-based coordination language. Science of Computer Programming, 31:359-381, 1998<br>
[7] Iain Merrick and Alan Wood. Scoped coordination in open distributed systems. In Proceedings of the 4th International Conferance on Coordination Languages and Models, pages 311-316. Springer-Verlag, 2000.<br>
[8] J. L. Jacob and A. M. Wood. A principled semantics for inp. In A. Porto and G.C. Roman, editos, Volume 1906 of Lecture Notes in Computer Science, pages 51-65, 2000.<br>
[9] C. A. R. Hoare. Communicating sequential processes. Prentice-Hall, Inc., 1985.<br>
[10] Antony Rowstron and Alan Wood. Solving the Linda multiple read problem. http://www.cs.york.ac.uk/linda/ps/coord.ps.gz.<br>
[11] Antony I. T. Rowstron and Alan Wood. Solving the Linda multiple rd problem using the copy-collect primitive. Science of Computer Programming, 31(2-3):335-358, 1998.<br>
[12] A. Rowstron and A. Wood.  An Efficient Distributed Tuple Space Implementation for Networks of Workstations.  In L. Bouge, P. Fraigniaud, A. Mignotte, and Y. Robert, editors, EuroPar 96, volume 1123, pages 511-513. Springer-Verlag, Berlin, 1996.<br>
[13] Sun Microsystems. JavaSpace Specification. http://java.sun.com/products/jini/specs, 1998.<br>
[14] Tobin J. Lehman, Stephen W. McLaughry, and Peter Wycko. T Spaces: The next wave. In HICSS, 1999.<br>
[15] Rafael D. Lins. Cyclic reference counting with lazy mark-scan. Information Processing Letters, 44(4):215-220, 1992.<br>
[16] David Plainfosse and Marc Shapiro. A survey of distributed garbage collection techniques. In IWMM, pages 211-249, 1995.<br>
[17] R. P. Menezes. Resource Management in Open Tuple Space Systems. PhD thesis, Department of Computer Science, University of York, 1999.<br>

*/
