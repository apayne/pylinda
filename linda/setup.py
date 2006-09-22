#!/usr/bin/env python
"""
Linda is an widely studied distributed computing environment, centred around
the notion of a tuple space. A tuple space is a bag (also called a multi-set) of
tuples. A tuple is an ordered, typed chunk of data. Tuple spaces exist
independently of processes in the system, and the data placed into a tuple space
also exist independently. See "Generative communication in Linda" (1985) and
"Multiple tuple spaces in Linda" both by David Gelernter for more information on
Linda.

PyLinda is a simple implementation of a linda system, however it also includes
several of the more recently proposed extensions to Linda in the form of multiple
tuple spaces, garbage collection, sane non-blocking primitives and bulk tuple
operations.
"""


from distutils.core import setup, Command, Extension
from distutils.command.install import install
from distutils import log
from distutils import sysconfig
import glob
import os
import re
import sys
from stat import ST_MODE

version = "0.6"

# check if Python is called on the first line with this expression
first_line_re = re.compile('^#!.*python[0-9.]*([ \t].*)?$')

def change_first_line(script, dry_run, warn):
    adjust = 0
    try:
        f = open(script, "r")
    except IOError:
        if not dry_run:
           raise
        f = None
    else:
        first_line = f.readline()
        if not first_line:
            warn("%s is an empty file (skipping)" % script)
            return

        match = first_line_re.match(first_line)
        if match:
            adjust = 1
        post_interp = match.group(1) or ''

    if adjust:
        log.info("adjusting %s", script)
        if not dry_run:
            lines = f.readlines()

            outf = open(script, "w")
            if not sysconfig.python_build:
                outf.write("#!%s%s\n" % (os.path.normpath(sys.executable), post_interp))
            else:
                outf.write("#!%s%s\n" % (os.path.join(sysconfig.get_config_var("BINDIR"),
                                         "python" + sysconfig.get_config_var("EXE")), post_interp))
            outf.writelines(lines[1:])
            outf.close()
        if f:
            f.close()
    else:
        f.close()

def getlibpath():
    l = sys.path
    l.reverse()
    for x in l:
        if x.startswith(sys.exec_prefix) and x.endswith("site-packages"):
            print "Installing data files in", x[len(sys.exec_prefix)+1:] + os.sep + "linda"
            return x[len(sys.exec_prefix)+1:] + os.sep + "linda"
    raise SystemError, "Unable to find site-packages directory"

import sys
if sys.platform == "win32":
    root_path = "pylinda"
else:
    root_path = "share/doc/pylinda-" + version

class myinstall(install):
    def run(self):
        install.run(self)

        print "PyLinda's post-install options"

        # the following is taken from the install_scripts standard command
        if os.name == 'posix':
            # Set the executable bits (owner, group, and world) on
            # all the scripts we just installed.
            if self.root is None:
                root = ""
            else:
                root = self.root
            for file in glob.glob(root + "/" + root_path + "/examples/*.py"):
                print file
                change_first_line(file, self.dry_run, self.warn)
                if self.dry_run:
                    log.info("changing mode of %s", file)
                else:
                    mode = ((os.stat(file)[ST_MODE]) | 0555) & 07777
                    log.info("changing mode of %s to %o", file, mode)
                    os.chmod(file, mode)

mycmdclass = {"install": myinstall}

import sys

ext = []

setup(name="linda",
      description="An implementation of the tuplespace based distributed computing system",
      long_description=__doc__,
      author="Andrew Wilkinson",
      author_email="aw@cs.york.ac.uk",
      url="http://www-users.cs.york.ac.uk/~aw/pylinda",
      download_url="http://www-users.cs.york.ac.uk/~aw/pylinda/linda-"+version+".tar.gz",
      license="Lesser GPL",
      platforms=["Linux", "Windows," "Mac OS X", "Solaris"],
      classifiers="""Development Status :: 4 - Beta
                     Environment :: Console
                     Intended Audience :: Science/Research
                     License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)
                     Natural Language :: English
                     Operating System :: POSIX :: Linux
                     Operating System :: MacOS :: MacOS X
                     Operating System :: POSIX :: SunOS/Solaris
                     Programming Language :: Python
                     Topic :: System :: Distributed Computing""".split("\n"),
      version=version,
      scripts=["linda_server"],
      data_files=[(root_path+"/examples", glob.glob("examples/*.py")),
                  (root_path+"/doc", glob.glob("doc/html/*")),
                  (root_path, ["README"]),
                  (getlibpath()+"/monitor", ['monitor/pyg_monitor.pyg', 'monitor/pyg_monitor.pyl', 'monitor/pyg_monitor_gramtab.py', 'monitor/pyg_monitor_lextab.py'])],
      ext_modules=ext,
      packages=["linda", "linda.monitor", "linda.monitor.pyggy"],
      package_dir={"linda": ""},
      cmdclass=mycmdclass)
