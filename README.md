# Client-Server Protocol

In this assignment, you will create a client and server in C that
implement a simple protocol. You can test this in your testbed, and
we will use it as the basis for the next assignment.

## Protocol Overview

The protocol you will implement involves clients requesting work
from the server, in the form of a number to factor. The server will
send a number to a requesting client, and the client will return
the factors to the server.

A client can request work from the server multiple times, but the
server will not send new work until the previous work is completed.

## Protocol Details

The protocol should implement the following messages. All values are in
hexadecimal.

| Message Type (1B)        | Contents                                 |
| ------------------------ | ---------------------------------------- |
| `01` Client handshake    | `0124`                                   |
| `02` Server handshake    | u16 client ID                            |
| `03` Client work request | u16 client ID                            |
| `04` Server work         | u32                                      |
| `05` Server waiting      | none                                     |
| `06` Client response     | u16 client ID; u16 length; [u32 factors] |

 * u16 refers to an unsigned 16-bit integer
 * u32 refers to an unsigned 32-bit integer
 * [u32 factors] refers to a variable-length (provided in the message) array

All numbers are in network byte order.

The server is free to choose client IDs and numbers to factor however it
likes. The client is free to determine whether to request additional work
or close the connection. The client ID supplied by the server must be
used by the client in subsequent requests.

The message exchanges are:

 1. Client sends a message `01` to the server on initial connection.
 2. Servers replies to a message `01` with a message `02` *if this is
    a new connection*. Otherwise, it closes the connection to the client.
 3. Client replies to a message `02` with a message `03`.
 4. Server replies to a message `03` with either:
    1. message `04` with new work if it is not waiting for work to complete
       by that client, or
    2. message `05` if the client has already requested work but not replied
       with a message `06`.
 5. Client receiving a message `04` factors the provided number, and replies
    with a message `06`.
 6. Optionally, the client requests more work with a message `03`.

Any unexpected messages received by a client or the server should terminate
the connection.

## Implementation

We have provided a function `factor` in `factor.c`, with a prototype
in `factor.h`. This function takes a `uint32_t` (in host byte order),
and returns a linked list of factors. You must free the memory used by
this list when you are done with it.

Your client should be contained in the file `client.c`, and must be
written in C. It should take two argument on the command line:
 1. The hostname or IP address of the server
 2. The port on which the server is listening

Your server should be contained in the file `server.c`, and must be
written in C. It should take a port to listen on as its only command-line
argument.

The server should handle multiple clients using `poll`.

Compilation should be done using a `Makefile`, which you must provide.

All source code, headers, and your `Makefile` must be committed in git
and pushed to your repository on the department gitlab server.

### Tips

 * You may use `srand` and `rand` to generate random numbers, if you like,
   but this is not necessary.
 * You will probably want to define a structure for each client's state
   on the server, so that you know where in the protocol any particular
   client is.
 * You will need to keep track of which client ID corresponds to which
   socket file descriptor.
 * The `nfds` parameter to `poll` must be the index *past* the last used
   entry in `fdarray`. Note that you do not have to use these names.
 * *Always* check the return values from any non-`void` function!

## Submission

Your files must be pushed to gitlab.cs.umd.edu by the deadline to
receive credit.

## Outcomes

Your grade for this assignment will be determined by the following
criteria. Each higher grade must satisfy all of the grades below it,
so to receive an A, you must satisfy all three sets of criteria.

**C** The client can connect and send data to the server.

**B** The client and server implement the protocol correctly.

**A** The server supports multiple clients at once with `poll`.
