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

## \namespace messages
## \brief This contains the definitions for the messages used to communicate internally.
##
## \author Andrew Wilkinson <aw@cs.york.ac.uk>

done = "DONE" # Generic ok message
dont_know = "DONT_KNOW" # Don't know the answer

register_process = "REGISTER_PROCESS" # Sent by a client process to get an id
register_thread = "REGISTER_THREAD" # Sent by a client thread to get an id
unregister_thread = "unregister_thread" # Sent by a client thread which is shuttind down

unregister_server = "unregister_server"

my_name_is = "MY_NAME_IS" # If a server connects and already has an id then this is sent

create_tuplespace = "CREATE_TUPLESPACE" # Sent by a client to create a tuplespace

get_connect_details = "GET_CONNECTION_DETAILS" # Sent to find out how to connect to a server
get_neighbours = "GET_NEIGHBOURS"
get_node_id = "GET_NODE_ID" # Asks the destination server what it's id is

rd_tuple = "RD" # Sent by a client process to read a tuple
in_tuple = "IN" # Sent by a client process to in a tuple
rdp_tuple = "RDP" # Sent by a client process to read a tuple
inp_tuple = "INP" # Sent by a client process to in a tuple
out_tuple = "OUT" # Sent by a client process to out a tuple
unblock = "UNBLOCK" # Return message to unblock a client process

return_tuple = "RETURN_TUPLE" # Return message when a tuple is being returned

collect = "collect" # Sent by a client process to collect tuples
copy_collect = "copy_collect" # Sent by a client process to copy tuples

multiple_in = "multiple_in" # Internal message to move a group of tuples

add_reference = "ADD_REFERENCE" # Sent to increment the reference count of a tuple space
delete_reference = "DELETE_REFERENCE" # Sent to decrement the reference count of a tuple space

tuple_request = "TUPLE_REQUEST" # Tell a server we're looking for tuples of this type
cancel_request = "cancel_request" # Cancel a tuple request
get_requests = "GET_REQUESTS"

has_tuplespace = "has_tuplespace"
is_deadlocked = "is_deadlocked"

get_stats = "get_stats" # Get the stats for a server

not_permitted = "not_permitted" # Action not permitted (not currently used - reserved for capablity linda)

kill_server = "kill_server"

yes = "yes"
no = "no"

close_connection = "close_connection"

monitor = "MONITOR"
list_ts = "LIST_TS"
inspect_ts = "INSPECT"
get_routes = "GET_ROUTES" # Get information on how to connect to all the other servers in the network.

register_partition = "REGISTER_PARTITION"
deleted_partition = "DELETED_PARTITION"
get_partitions = "GET_PARTITIONS"
