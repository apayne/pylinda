/**

\page todo To Do List

PyLinda is still under development and there is stil much work to be done. This page lists all the work that is being considered for inclusion in PyLinda.

<ul>
<li><b>Server Interface</b> The basic interface for the Linda server needs expanding and more commands adding to it.</li>

<li><b>Cleanly shutting down the server</b> As Python offers no way to explicitly kill a thread shutting down the server is an error prone process that often results in exceptions being thrown. This needs to be improved so shutting down the server is a clean process.</li>

<li><b>Eval primitive</b> PyLinda is missing the eval primitive from Linda, while tricky to implement it may be worth the effort.</li>

<li><b>Tuple Space distribution</b> PyLinda current only stores a tuplespace on a single node, they should be spread across the network with some form of load balancing.

<li><b>Other languages</b> Currently only Python may be used to write client applications. This will change to allow C, C++ and Haskell.

</ul>

*/
