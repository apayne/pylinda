/**

\page download Download

\section CurrentVersion Current Version

The current version of PyLinda is 0.6.

\li To download PyLinda in \c .tar.gz format, click <a href="dist/linda-0.6.tar.gz">here</a>

After downloading the file it should be extracted and then the following commands run.
\code
cd linda-0.6
python setup.py install
\endcode

\section Gentoo Gentoo Ebuild

Gentoo users can follow the following instructions to install PyLinda using the Portage system.

\code
mkdir -p /usr/local/portage/dev-python/pylinda
cd /usr/local/portage/dev-python/pylinda
wget http://www-users.cs.york.ac.uk/~aw/pylinda/pylinda-0.6.ebuild
ebuild pylinda-0.6.ebuild digest
echo "=dev-python/pylinda-0.5.1 ~x86" >> /etc/portage/package.keywords
emerge pylinda
\endcode

\image html powered-by-gentoo.jpg

\section Windows Windows

The Windows version of Python does not come with the readline module. While PyLinda will run fine 
without it the interface to the server will be restricted. A version of this module that works with 
Windows can be downloaded from <a href="http://newcenturycomputers.net/projects/readline.html">here</a>. (Thanks to Pablo Grigoletti for pointing this out.)

\section PreviousVersion Previous Versions

\li To download PyLinda V0.5.1 click <a href="dist/linda-0.5.1.tar.gz">here</a>
\li To download PyLinda V0.5 click <a href="dist/linda-0.5.tar.gz">here</a>
\li To download PyLinda V0.4.1 click <a href="dist/linda-0.4.1.tar.gz">here</a>
\li To download PyLinda V0.4 click <a href="dist/linda-0.4.tar.gz">here</a>
\li To download PyLinda V0.3 click <a href="dist/linda-0.3.tar.gz">here</a>
\li To download PyLinda V0.2 click <a href="dist/linda-0.2.tar.gz">here</a>
\li To download PyLinda V0.1 click <a href="dist/linda-0.1.tar.gz">here</a>

*/
