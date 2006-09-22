/**
\example marriage.py

This example solves the stable marriages problem.

\code
marriage.py men
marriage.py women
\endcode

This example solves the <a href="http://mathworld.wolfram.com/StableMarriageProblem.html">stable marriages problem</a>. A classic distributed computing problem involves a group of men and a group of women. Each person has an ordered list ranking each member of the opposite sex in the order they want to marry them. A stable marriage is one where there are no pairs of the form {w<sub>i</sub>, m<sub>j</sub>} {w<sub>k</sub>, m<sub>t</sub>} where w<sub>i</sub> prefers m<sub>t</sub> to m<sub>j</sub> and m<sub>t</sub> prefers w<sub>i</sub> to w<sub>k</sub>.

This example is based on the example in <i>A Principled Semantics for inp</i> by Jacob & Wood 2000. The main code is in the Men and Women functions. The rest of the code is just to fork a seperate process for each person in the simulation.

The code itself is very simple, and relies mostly on the fact that when deadlock is reached the algorithm has finished. The inp calls detect the deadlock and cause the processes to exit.

*/
