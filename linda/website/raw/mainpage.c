/**
\mainpage PyLinda - Distributed Computing Made Easy

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

\section News

<ul>
<li>12/7/05 - <b>Updated install script for 0.5</b><br>
There was a small bug in the install script for version 0.5 that prevented it from installing properly. Please download the package again and reinstall if you were having problems with an exception when running linda_server. Thanks to Pablo Souza Grigoletti for the bug report.
<li>10/7/05 - <b>GoogleGroup created.</b><br>
There is now a GoogleGroups discussion forum for discussing anything
related to PyLinda. Click <a
href="http://groups-beta.google.com/group/pylinda">here</a> to join or
view the archives.
</ul>

\section ChangeLog History

<ul>
<li>6/2/06 - Version 0.6
 <ul><li>Bug Fixes
  <ul>
  <li>The method used to determine whether to use unix domain sockets or TCP/IP sockets has been replaced.
  <li>Install script changed to install documentaton in the standard location.
  <li>Various minor fixes.
 </ul>
 <li>New Features
  <ul>
  <li>Client-side programs can now use multiple threads to access to the Linda system.
  <li>Added a 'uts' alias for the universal tuplespace.
  <li>Added a disconnect method which allows processes and threads to disconnect from the Linda network.
  </ul>
 </ul>
<li>14/7/05 - Version 0.5.1
 <ul><li>Bug Fixes
  <ul>
  <li>Fixed a serious bug that would cause PyLinda to randomly deadlock when two or more servers were used. Thanks to Pablo Grigoletti for providing a test case.
  <li>Made the readline module optional as Windows Python doesn't supply it by default. Thanks to Pablo Grigoletti for pointing this out.
  <li>Various fixes to the install script.
 </ul>
 </ul>
<li>21/6/05 - Version 0.5
 <ul><li>New Features
  <ul>
  <li>Renamed the server to simply 'linda_server'. Using the .py extension will no longer work.
  <li>Removed the assumption that all servers can make a direct connection to every other server. This will initially slow things down a bit, but hopefully this can be improved. It lets you do cool stuff like linking two seperate groups of servers over an ssh tunnel.
  <li>Added a simple text interface to the server, type help for a list of commands. This needs adding to the documentation.
 </ul>
 <li>Bug Fixes
 <ul>
  <li>Various minor corrections.
 </ul>
 </ul>
<li>24/3/05 - Version 0.4.1
 <ul><li>Bug Fixes
  <ul>
  <li>Fixed a bug where a collect from a tuplespace on a remote server would fail. Thanks to Dr. Alan Wood for the bug report.
  <li>Fixed a bug in the install script where it would fail when installing in a nonstandard prefix. It's still not perfect, hopefully Version 0.5 will have a much better script. Thanks to Dr. Alan Wood for the bug report.
  </ul>
 </ul>
<li>13/1/05 - Version 0.4
 <ul><li>New Features
  <ul>
  <li>Removed SysV shared memory and made Unix Domain Sockets the default as they are quicker.
  <li>A new flashy website
  </ul>
 </ul>
 <ul><li>Bug Fixes
  <ul>
   <li>Fix bug where some sockets would not be in blocking mode and hence cause an exception. Thanks to Dave St.Germain for the bug report.
   <li>Fix bug where Windows didn't define MSG_DONTWAIT in the socket module. Thanks to Dr Andrew Perella for the bug report.
   <li>Fix bug that prevented a network of servers from being formed. Thanks to Igor Chaves Cananea for the bug report and patch.
  </ul>
 </ul>
</li>
<li>2/7/04 - Version 0.3
 <ul><li>Efficency Updates
  <ul><li>Switched to using System V Shared Memory where avilable.</li>
  <li>Switched to using Unix domain sockets where available.</li>
  <li>Switched to using map instead of an explict loop in several places</li>
  <li>Changed main message handler to a dictionary of functions instead of big if/elif statement</li></ul>
 </li></ul>
</li>

<li>3/5/04 - Version 0.2
 <ul>
  <li>New features
  <ul>
   <li>Patch from Andreas Kostyrka to allow the server to be bound only to localhost preventing any external connections. Run as 'linda_server.py -a' to prevent any external access to the server.</li>
   <li>Patch from Andreas Kostyrka to allow limiting the server to certain IP addresses/ranges. Run as 'linda_server.py --peer=<dns name/ip address/ip range>' where ip range is of the form n.n.n.n/r.</li>
  </ul>
  </li>
  <li>Bug Fixes
   <ul>
   <li>Thanks to Andreas Kostyrka for spotting that printing the universal tuplespace caused an exception.</li>
   <li>Fixed bug in tuple container that could cause a type error</li>
   </ul>
  </li>

  <li>Efficency Updates
   <ul>
   <li>Modify the client-side code so messages where the return message from the server is not important don't block. If another message is sent to the server before the return message is received then that message blocks.</li>
   </ul>
  </li>

  <li>Packaging Updates
   <ul>
   <li>Change examples and documentation to install under /usr/share/pylinda</li>
   <li>Set example programs to be marked as executable and have the shebang lines modified to the local python install.</li>
   </ul>
  </li>

  <li>Documentation Updates
   <ul>
   <li>Distributed documentation does not now contain the internal structure of PyLinda.</li>
   <li>Moved documentation to a proxy c file which is easier for Doxygen to parse.</li>
   <li>Changed "How To Install" to refer to the website, not my internal cvs tree.</li>
   <li>Added a tutorial</li>
   <li>Better code commenting + a small tidy up</li>
   </ul>
  </li>
 </ul>
</li>

<li>29/4/04 - Version 0.1
 <ul>
  <li>Initial release</li>
 </ul>
</li>
</ul>

*/
