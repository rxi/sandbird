#ifndef _WIN32
#define _BSD_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sandbird.h"


static void write_error_page(sb_Stream *st, const char *text) {
  sb_writef(st, "<html><body><h2>Error: %s</h2></body></html>\n", text);
}


static int event_handler(sb_Event *e) {
  if (e->type == SB_EV_REQUEST) {
    const char *path;
    struct stat s;
    int err;

    /* Log request */
    printf("%s - %s %s\n", e->address, e->method, e->path);

    /* Is the requested path valid? */
    if (e->path[1] == '/' || strstr(e->path, ":/") || strstr(e->path, "..")) {
      sb_send_status(e->stream, 400, "Bad request");
      write_error_page(e->stream, "bad request");
      return SB_RES_OK;
    }

    /* Convert to filesystem path */
    if (e->path[1] == '\0') {
      path = ".";
    } else {
      path = e->path + 1;
    }

    /* Get file info */
    err = stat(path, &s);
    /* Does file / dir exist? */
    if (err == -1) {
      sb_send_status(e->stream, 404, "Not found");
      write_error_page(e->stream, "page not found");
      return SB_RES_OK;
    }

    /* Handle directory */
    if (S_ISDIR(s.st_mode)) {
      DIR *dir;
      struct dirent *ent;
      /* Send page header */
      sb_writef(e->stream, 
                "<html><head><title>Directory listing for %s</title></head>"
                "<body><h2>Directory listing for %s</h2><hr><ul>",
                e->path, e->path);
      /* Send file list */
      dir = opendir(path);
      while ((ent = readdir (dir)) != NULL) {
        if (!strcmp(ent->d_name, ".")) continue;
        if (!strcmp(ent->d_name, "..")) continue;
        sb_writef(e->stream, "<li><a href=\"/%s/%s\">%s%s</a></li>",
                  path, ent->d_name, ent->d_name,
                  ent->d_type == DT_DIR ? "/" : "");
      }
      closedir(dir);
      /* Send page footer */
      sb_writef(e->stream, "</ul></body></html>");
      return SB_RES_OK;
    }

    /* Handle file */
    err = sb_send_file(e->stream, path);
    if (err) {
      sb_send_status(e->stream, 500, "Internal server error"); 
      write_error_page(e->stream, sb_error_str(err));
    }
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
