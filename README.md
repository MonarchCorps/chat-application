# xchat — Terminal Chat Application in C

A multi-client terminal chat application built from scratch in C. Supports real-time messaging between multiple clients over TCP sockets using `select()` for I/O multiplexing.

Built as Project 14 of a systems programming curriculum, following projects covering memory allocators, file explorers, process monitors, text editors, and HTTP servers.

---

## Features

- Multi-client support — up to 50 simultaneous connections
- Real-time broadcast — messages delivered to all connected clients instantly
- Username system — every client identifies with a username on connect
- Join/leave notifications — all clients are notified when someone connects or disconnects
- Non-blocking I/O — single-process server using `select()`, no threads or forking
- Clean disconnect handling — server recovers gracefully when clients drop

---

## Architecture

```
xchat/
├── server.c          # Server logic — select() loop, broadcast, client tracking
├── server_main.c     # Server entry point
├── client.c          # Client logic — select() on stdin + server socket
├── client_main.c     # Client entry point
└── CMakeLists.txt    # Build configuration
```

Two separate binaries:
- `xchat_server` — runs on the host machine, manages all connections
- `xchat_client` — connects to the server, one instance per user

---

## How It Works

### Server

The server uses a single `select()` loop to monitor multiple file descriptors simultaneously — no threads, no forking.

```
socket() → bind() → listen()

while (1):
    rebuild fd_set from all connected client fds + listening socket
    select() — block until any fd has activity

    if listening socket ready → accept() new client, read username, broadcast join notice
    for each client fd:
        if ready → read message, broadcast to all others
        if disconnected → broadcast leave notice, remove from array
```

Client state is tracked in two parallel arrays:
- `int clients[MAX_CLIENTS]` — file descriptors, initialized to -1
- `char usernames[MAX_CLIENTS][256]` — username per slot

### Client

The client also uses `select()` to watch two fds at once:
- `stdin` (fd 0) — user typed something → send to server
- `server_fd` — server pushed a message → print it

This allows the client to receive messages from other users at any time, without blocking on input.

---

## Message Protocol

Newline-delimited plain text over TCP.

- First message after connect → username
- All subsequent messages → chat content
- Server prefixes messages: `[username]: message`
- Server sends system notices: `*** username has joined the chat ***`

---

## Build

**Requirements:**
- GCC or Clang
- CMake 3.10+
- macOS or Linux

```bash
cmake -B build
cmake --build build
```

This produces two executables in `build/`:
- `build/xchat_server`
- `build/xchat_client`

---

## Usage

**Terminal 1 — start the server:**
```bash
./build/xchat_server
```

**Terminal 2+ — connect clients:**
```bash
./build/xchat_client
```

Each client is prompted for a username on connect. After that, type and hit Enter to send.

**Example session:**

Client 1 (david):
```
Enter username: david
*** alice has joined the chat ***
[alice]: yo what's good
hello from david
```

Client 2 (alice):
```
Enter username: alice
yo what's good
[david]: hello from david
```

Server terminal:
```
*** david has joined the chat ***
*** alice has joined the chat ***
[alice]: yo what's good
[david]: hello from david
```

---

## Key Concepts

| Concept | Where Used |
|---|---|
| `select()` / `fd_set` | Server and client I/O multiplexing |
| `socket()` / `bind()` / `listen()` / `accept()` | TCP server setup |
| `connect()` | Client connection |
| `SO_REUSEADDR` | Prevent "address already in use" on restart |
| `inet_pton()` | Convert IP string to binary |
| `htons()` | Host to network byte order |
| Parallel arrays | Track client fds and usernames together |
| Newline framing | Simple message protocol over TCP byte stream |

---

## Limitations

- No message history — new clients only see messages sent after they join
- No encryption — plain text over TCP
- Local network only by default — server binds to `INADDR_ANY`, client connects to `127.0.0.1`
- Max 50 clients — defined by `MAX_CLIENTS`

---

## Part of a Larger Journey

xchat is one project in a series building toward **EduOS** — a privacy-first, AI-native Linux-based OS for African schools. Offline. Adaptive. No tracking.

The systems programming skills built here — sockets, multiplexing, process management, memory allocation — are the foundation that OS will run on.

---

## Author

**Okocha David** — Full Stack Engineer & Systems Programming Student  
Lagos, Nigeria  
X: [@davidokocha086](https://x.com/davidokocha086)