from howl import *
import time
import thread

from linda.options import getOptions

session = None

def connect():
    global session

    session = sw_discovery_init()

    servers = {}

    def resolve(discovery, oid, interface_index, name, type, domain, address, port, text_record, text_record_len, extra):
        servers[".".join([str(x) for x in [address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF, (address >> 24) & 0xFF]])] = port
        return SW_OKAY
    def displayreply(discovery, oid, status, interface_index, name, type, domain, extra):
        r, oid = sw_discovery_resolve(discovery, interface_index, name, type, domain, resolve, extra)
        return SW_OKAY

    print "Browsing for existing servers..."
    r, oid = sw_discovery_browse(session, 0, "_pylinda._tcp", None, displayreply, None)
    thread.start_new_thread(sw_discovery_run, (session, ))
    time.sleep(10)
    sw_discovery_cancel(session, oid)
    sw_discovery_stop_run(session)

    print "Finished browse, found %i servers" % (len(servers), )

    def displaystatus(discovery, oid, status, extra):
        print status
        return SW_OKAY

    print "Publishing server..."
    r, oid = sw_discovery_publish(session, 0, "PyLinda", "_pylinda._tcp", None, None, getOptions().port, displaystatus, None)
    thread.start_new_thread(sw_discovery_run, (session, ))
    #time.sleep(10)
    #sw_discovery_cancel(session, oid)
    #sw_discovery_stop_run(session)
    print "Zeroconf start up complete"

    #sw_discovery_fina(session)

    return servers.keys(), servers.values()

def disconnect():
    sw_discovery_fina(session)