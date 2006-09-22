#    Copyright 2004 Andrew Wilkinson <aw@cs.york.ac.uk>.
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
import threading

import connections

try:
    socket.AF_UNIX
except AttributeError:
    raise ImportError

class LindaDomainServer(SocketServer.ThreadingMixIn, SocketServer.UnixStreamServer):
    def __init__(self, address, allowed_peers=[]):
        """\internal
        """
        if(os.path.exists(address)):
            os.unlink(address)
        self.address = address
        SocketServer.UnixStreamServer.__init__(self, address, None)
        self.daemon_threads = True
        self.allowed_peers = allowed_peers
        self.close = False

    def serve_forever(self):
        """Handle one request at a time until doomsday."""
        try:
            while not self.close:
                self.handle_request()
        finally:
            os.unlink(self.address)

    def handle_request(self):
        """Handle one request, possibly blocking."""
        try:
            self.socket.settimeout(1.0)
            request, client_address = self.get_request()
            self.socket.settimeout(None)
        except socket.error:
            return
        if self.verify_request(request, client_address):
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
