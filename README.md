# Sandbird
A tiny (~800sloc) embeddable HTTP server written in C89, compatible with Linux,
OSX and Windows.


## Getting started
Sandbird consists of two files: [sandbird.c](src/sandbird.c?raw=1) and
[sandbird.h](src/sandbird.h?raw=1). Both of these files should be dropped into
an existing project and compiled along with it. Sandbird works asynchronously
and does not use threads, an event handler is invoked when an event -- such as
a request -- occurs.
```c
static int event_handler(sb_Event *e) {
  if (e->type == SB_EV_REQUEST) {
    sb_send_status(e->stream, 200, "OK");
    sb_send_header(e->stream, "Content-Type", "text/plain");
    sb_writef(e->stream, "Hello world");
  }
  return SB_RES_OK;
}
```

The server is initialised by setting up a `sb_Options` struct and passing a
pointer to it to `sb_new_server()`. At a minimum the `port` and `handler`
fields of the `sb_Options` struct should be set.
```c
sb_Server *srv;
sb_Options opt;

memset(&opt, 0, sizeof(opt));
opt.port = "8000";
opt.handler = event_handler;

srv = sb_new_server(&opt);
```

If `sb_new_server()` encounters an error, `NULL` is returned -- in this case it
is most likely the socket could not be bound. A non-NULL return value means the
server started successfully and is listening for connections.

The poll function must be called on the server object regularly so that it can
handle new and existing connections. The poll function accepts a timeout value
in milliseconds -- if this is set to `0` then the function will not block.
```c
for (;;) {
  sb_poll_server(srv, 1000);
}
```

When we are done with the server the `sb_close_server()` function can be called
on it. This will close the server's listening socket and free any resources
which were being used.
```c
sb_close_server(srv);
```

Some simple example programs can be found in the [example/](example/)
directory.


## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.
