#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sandbird.h"


static int event_handler(sb_Event *e) {
  if (e->type == SB_EV_REQUEST) {
    printf("%s - %s %s\n", e->address, e->method, e->path);
    sb_send_status(e->stream, 200, "OK");
    sb_send_header(e->stream, "Content-Type", "text/plain");
    sb_writef(e->stream, "Hello world");
  }
  return SB_RES_OK;
}


int main(void) {
  sb_Options opt;
  sb_Server *server;

  memset(&opt, 0, sizeof(opt));
  opt.port = "8000";
  opt.handler = event_handler;

  server = sb_new_server(&opt);

  if (!server) {
    fprintf(stderr, "failed to initialize server\n");
    exit(EXIT_FAILURE);
  }

  printf("Server running at http://localhost:%s\n", opt.port);

  for (;;) {
    sb_poll_server(server, 1000);
  }

  sb_close_server(server);
  return EXIT_SUCCESS;
}
