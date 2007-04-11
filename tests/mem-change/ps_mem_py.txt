#!/usr/bin/env python


# Try to determine how much RAM is currently being used per program.
# Note the per program, not per process. So for example this script
# will report mem used by all httpd process together. In detail it reports:
# sum(all RSS for process instances) + max(shared mem for any process instance)
#
# The shared calculation below will factor out shared text and
# libs etc. within a program, but not between programs. So there
# will always be some overestimation. This will be the same for
# all processes that just use libc for e.g. but more for others
# that use larger shared libs like gnome, kde etc.


# Author: P@draigBrady.com

# V1.0      06 Jul 2005     Initial release
# V1.1      11 Aug 2006     root permission required for accuracy
# V1.2      08 Nov 2006     Add total to output
#                           Use KiB,MiB,... for units rather than K,M,...
# V1.3      22 Nov 2006     Ignore shared col from /proc/$pid/statm for
#                           2.6 kernels up to and including 2.6.9.
#                           There it represented the total file backed extent
# V1.4      23 Nov 2006     Remove total from output as it's meaningless
#                           (the shared values overlap with other programs).
#                           Display the shared column. This extra info is
#                           useful, especially as it overlaps between programs.

# Notes:
#
# All interpreted programs where the interpreter is started
# by the shell or with env, will be merged to the interpreter
# (as that's what's given to exec). For e.g. all python programs
# starting with "#!/usr/bin/env python" will be grouped under python.
# You can change this by changing comm= to args= below but that will
# have the undesirable affect of splitting up programs started with
# differing parameters (for e.g. mingetty tty[1-6]).
#
# For 2.6 kernels up to and including 2.6.13 and later 2.4 redhat kernels
# (rmap vm without smaps) it can not be accurately determined how many pages
# are shared between processes in general or within a program in our case:
# http://lkml.org/lkml/2005/7/6/250
# A warning is printed if overestimation is possible.
# In addition for 2.6 kernels up to 2.6.9 inclusive, the shared
# value in /proc/$pid/statm is the total file-backed extent of a process.
# We ignore that, introducing more overestimation, again printing a warning.
#
# I don't take account of memory allocated for a program
# by other programs. For e.g. memory used in the X server for
# a program could be determined, but is not.
#
# This script assumes threads are already merged by ps

# TODO:
#
# use ps just to enumerate the pids and names
# so as to remove the race between reading rss and shared values

import sys, os, string

if os.geteuid() != 0:
    sys.stderr.write("Sorry, root permission required.\n");
    sys.exit(1)

PAGESIZE=os.sysconf("SC_PAGE_SIZE")/1024 #KiB
our_pid=os.getpid()

#(major,minor,release)
def kernel_ver():
    kv=open("/proc/sys/kernel/osrelease").readline().split(".")[:3]
    for char in "-_":
        kv[2]=kv[2].split(char)[0]
    return (int(kv[0]), int(kv[1]), int(kv[2]))

kv=kernel_ver()

def getShared(pid):
    if os.path.exists("/proc/"+str(pid)+"/smaps"):
        shared_lines=[line
                      for line in open("/proc/"+str(pid)+"/smaps").readlines()
                      if line.find("Shared")!=-1]
        return sum([int(line.split()[1]) for line in shared_lines])
    elif (2,6,1) <= kv <= (2,6,9):
        return 0 #lots of overestimation, but what can we do?
    else:
        return int(open("/proc/"+str(pid)+"/statm").readline().split()[2])*PAGESIZE

cmds={}
shareds={}
for line in os.popen("ps -o rss=,pid=,comm= %i" % (int(sys.argv[1], ))).readlines():
    size, pid, cmd = map(string.strip,line.strip().split(None,2))
    if int(pid) == our_pid:
        continue #no point counting this process
    try:
        shared=getShared(pid)
    except:
        continue #ps gone away
    if shareds.get(cmd):
        if shareds[cmd] < shared:
            shareds[cmd]=shared
    else:
        shareds[cmd]=shared
    #Note shared is always a subset of rss (trs is not always)
    cmds[cmd]=cmds.setdefault(cmd,0)+int(size)-shared

#Add max shared mem for each program
for cmd in cmds.keys():
    cmds[cmd]=cmds[cmd]+shareds[cmd]

sort_list = cmds.items()
sort_list.sort(lambda x,y:cmp(x[1],y[1]))
sort_list=filter(lambda x:x[1],sort_list) #get rid of zero sized processes (kernel threads)

#The following matches "du -h" output
#see also human.py
def human(num, power="Ki"):
    powers=["Ki","Mi","Gi","Ti"]
    while num >= 1000:
        num /= 1024.0
        power=powers[powers.index(power)+1]
        human(num,power)
    return "%.1f %s" % (num,power)

for cmd in sort_list:
    print "%s" % (cmd[1], )

#Warn of possible inaccuracies
#1 = accurate
#0 = some shared mem not reported
#-1= all shared mem not reported
def shared_val_accurate():
    """http://wiki.apache.org/spamassassin/TopSharedMemoryBug"""
    if kv[:2] == (2,4):
        if open("/proc/meminfo").read().find("Inact_") == -1:
            return 1
        return 0
    elif kv[:2] == (2,6):
        if os.path.exists("/proc/"+str(os.getpid())+"/smaps"):
            return 1
        if (2,6,1) <= kv <= (2,6,9):
            return -1
        return 0
    else:
        return 1

vm_accuracy = shared_val_accurate()
if vm_accuracy == -1:
    sys.stderr.write("Warning: Shared memory is not reported by this system.\n")
    sys.stderr.write("Values reported will be too large.\n")
elif vm_accuracy == 0:
    sys.stderr.write("Warning: Shared memory is not reported accurately by this system.\n")
    sys.stderr.write("Values reported could be too large.\n")

