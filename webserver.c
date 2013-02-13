// webserver.c

#include "webserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ev.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

// TODO What data size shall we use ?
#define MAXDATASIZE 1024

// The time between a connection has been accepted till the client sends something
#define TIMEOUT  15

struct watcher_data {
   char ip[INET6_ADDRSTRLEN];
   ev_timer *timeout;
};

struct timeout_data {
   ev_io *watcher;
};

// Get a socket to listen on
static int get_socket(char *port) {
   int status, sockfd;
   struct addrinfo hints;
   struct addrinfo *servinfo, *p;

   // Clear struct and set requirements for socket
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;     // IPv4/IPv6
   hints.ai_socktype = SOCK_STREAM; // TCP Stream
   hints.ai_flags = AI_PASSIVE;     // Wildcard address

   // Get address infos we later use to open socket with
   if ((status = getaddrinfo(NULL, "http", &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
   }

   // Loop through results and bind to first
   // TODO Should we open a socket for each, or just the first ?
   for (p = servinfo; p != NULL; p=p->ai_next) {

      // Create socket
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
         perror("socket");
         continue;
      }

      // Bind to socket
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) != 0) {
         close(sockfd);
         perror("bind");
         continue;
      }

      break;
   }

   // Check if we binded to anything
   if (p == NULL) {
      fprintf(stderr, "failed to bind\n");
      exit(1);
   }

   // Clean up
   freeaddrinfo(servinfo);

   // Listen on socket
   if (listen(sockfd, SOMAXCONN) < 0) {
      perror("listen");
      exit(1);
   }

   return sockfd;
}

// Get sockaddr, IPv4 or IPv6
static void *get_in_addr(struct sockaddr *sa)
{
   if (sa->sa_family == AF_INET) {
      // IPv4
      return &(((struct sockaddr_in*)sa)->sin_addr);
   } else {
      // IPv6
      return &(((struct sockaddr_in6*)sa)->sin6_addr);
   }
}

// Stop a watcher, close its file descripter, and free its data and it
static void kill_watcher(struct ev_loop *loop, struct ev_io *watcher) {
   ev_io_stop(loop, watcher);
   if (close(watcher->fd) != 0) {
      perror("close");
   }
   free(watcher->data);
   free(watcher);
}

static void kill_timeout(struct ev_loop *loop, struct ev_timer *timeout)
{
   ev_timer_stop(loop, timeout);
   free(timeout->data);
   free(timeout);
}

// Read header
static void hd_reader(struct ev_loop *loop, struct ev_io *watcher, int revents) {
   int bytes;
   char buffer[MAXDATASIZE];
   struct watcher_data *data = watcher->data;

   kill_timeout(loop, data->timeout);

   // Receive some data
   if ((bytes = recv(watcher->fd, buffer, MAXDATASIZE-1, 0)) < 0) {
      perror("recv");
      // TODO Handle errors better - look up error# etc.
      kill_watcher(loop, watcher);
      return;
   } else if (bytes == 0) {
      fprintf(stderr, "connection closed by %s\n", data->ip);
      kill_watcher(loop, watcher);
      return;
   }
   else
   {
      buffer[bytes] = '\0';

      // Print message
      printf("%s\n", buffer);

      // Send hello back
      if (send(watcher->fd, "\n\nHello, world!", 15, 0) == -1)
         perror("send");

      // TODO Currently we kill the watcher, this is just temporarily. Thats why we have it inside the else..
      kill_watcher(loop, watcher);
   }
}

// Timeout handler
static void timeout_cb(struct ev_loop *loop, struct ev_timer *timer, int revents)
{
   ev_io *watcher = ((struct timeout_data *) timer->data)->watcher;

   printf("timeout: %s\n",((struct watcher_data *) watcher->data)->ip);

   kill_watcher(loop, watcher);
   kill_timeout(loop, timer);
}

// Accept a connection
static void accept_conn(struct ev_loop *loop, struct ev_io *watcher, int revents) {
   int in_fd;
   socklen_t in_size;
   struct sockaddr_storage in_addr;
   char s[INET6_ADDRSTRLEN];
   struct ev_io *io_hd_watcher;
   struct ev_timer *timeout;

   // Accept connection
   in_size = sizeof in_addr;
   if ((in_fd = accept(watcher->fd, (struct sockaddr *)&in_addr, &in_size)) < 0) {
      perror("accept");
      return;
   }

   // Print a nice message
   inet_ntop(in_addr.ss_family, get_in_addr((struct sockaddr *)&in_addr), s, sizeof s);
   printf("got connection from %s\n", s);

   // Create timeout timer
   struct timeout_data *tData = malloc(sizeof(struct timeout_data));
   timeout = malloc(sizeof(struct ev_timer));
   timeout->data = tData;

   // Create new watcher with custom data for this connection
   struct watcher_data *wData = malloc(sizeof(struct watcher_data));
   strcpy(wData->ip, s);

   io_hd_watcher = malloc(sizeof(struct ev_io));
   io_hd_watcher->data = wData;

   // Set up the timeout and watcher references
   tData->watcher = io_hd_watcher;
   wData->timeout = timeout;

   // Start timeout and watcher
   ev_io_init(io_hd_watcher, hd_reader, in_fd, EV_READ);
   ev_timer_init(timeout, timeout_cb, TIMEOUT, 0.);

   ev_io_start(loop, io_hd_watcher);
   ev_timer_start(loop, timeout);

   // TODO Create timeout watcher
}

// Start the webserver
void ws_start(char *port) {
   struct ev_loop *loop = EV_DEFAULT;
   struct ev_io io_conn_watcher;

   int sockfd = get_socket(port);

   ev_io_init(&io_conn_watcher, accept_conn, sockfd, EV_READ);
   ev_io_start(loop, &io_conn_watcher);

   ev_run(loop, 0);

   if (close(sockfd) != 0) {
      perror("close");
   }
}

// Stop the webserver
void ws_stop() {
   ev_break(EV_DEFAULT, EVBREAK_ALL);
}

