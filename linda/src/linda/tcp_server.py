#    Copyright 2004-2006 Andrew Wilkinson <aw@cs.york.ac.uk>
#
#    This file is part of PyLinda (http://www-users.cs.york.ac.uk/~aw/pylinda)
#
#    PyLinda is free software; you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation; either version 2.1 of the License, or
#    (at your option) any later version.
#
#    PyLinda is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with PyLinda; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import os
import socket
import SocketServer
SocketServer.TCPServer.allow_reuse_address = True
import threading

import connections

class LindaTCPServer(SocketServer.ThreadingTCPServer):
    """\internal
    A simple class the implements a threaded socket server - using the ThreadingTCPServer class provided by Python
    """
    def __init__(self, address, allowed_peers=[]):
        """\internal
        """
        SocketServer.ThreadingTCPServer.__init__(self, address, None)
        self.daemon_threads = True
        self.allowed_peers = allowed_peers
        self.close = False

    def serve_forever(self):
        """Handle one request at a time until doomsday."""
        while not self.close:
            self.handle_request()

    def handle_request(self):
        """Handle one request, possibly blocking."""
        try:
            self.socket.settimeout(1.0)
            request, client_address = self.get_request()
            self.socket.settimeout(None)
        except socket.error:
            return
        if self.verify_request(request, client_address) and self.check_request(request, client_address):
            try:
                self.process_request(request, client_address)
            except:
                self.handle_error(request, client_address)
                self.close_request(request)

    def process_request(self, request, client_address):
        """Start a new thread to process the request."""
        request = connections.Connection(request)
        request.type = None
        connections.sockets.append(request)

    def check_request(self, request, addr):
        # check that the connection comes from an allowed source
        if self.allowed_peers:
            ok = False
            for addr in self.allowed_peers:
                if self.verify_address(addr):
                    ok = True
                    break
            if not ok:
                print "DENIED ACCESS FROM", self.client_address
                self.request.shutdown(1)
                self.request.close()
                return False
        return True

    def verify_address(self, addr):
        flds = addr.split("/")
        addr = flds[0]
        if len(flds) == 1:
            mask = 32
        elif len(flds) == 2:
            mask = int(flds[1])

        naddr = struct.unpack("!i", socket.inet_aton(addr))[0] & utils.mask(mask)
        client = struct.unpack("!i", socket.inet_aton(socket.gethostbyname(self.client_address[0])))[0] & utils.mask(mask)
        return naddr == client
