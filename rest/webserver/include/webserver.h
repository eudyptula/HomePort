// webserver.h

/*  Copyright 2013 Aalborg University. All rights reserved.
 *   
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY Aalborg University ''AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Aalborg University OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  The views and conclusions contained in the software and
 *  documentation are those of the authors and should not be interpreted
 *  as representing official policies, either expressed.
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "ws_types.h"

#include <stddef.h>
#include <stdarg.h>

// Structs
struct ev_loop;
struct ws;
struct ws_conn;

/**********************************************************************
 *  Callbacks                                                         *
 **********************************************************************/

typedef int  (*ws_nodata_cb)(struct ws *instance, struct ws_conn *conn,
                             void *ws_ctx, void **data);
typedef int  (*ws_data_cb)  (struct ws *instance, struct ws_conn *conn,
                             void *ws_ctx, void **data,
                             const char *buf, size_t len);

/// Settings struct for webserver
/**
 *  Please initialise this struct as following, to ensure that all
 *  settings have acceptable default values:
 *  \code
 *  struct ws_settings *settings = WS_SETTINGS_DEFAULT;
 *  \endcode
 *
 *  The settings hold a series of callbacks of type either data_cb or
 *  nodata_cb. Do not expect the string parameter in data callbacks to
 *  be null terminated. All data callbacks are called on chunks and
 *  therefore may be called multiple times. It is up to the implementer
 *  of these callbacks to concatenate the results if needed.
 *
 *  The callbacks are called in the following order:
 *  \dot
 *  digraph callback_order {
 *  on_connect -> on_receive;
 *  on_connect -> on_disconnect;
 *  on_receive -> on_receive;
 *  on_receive -> on_disconnect;
 *  }
 *  \enddot
 *
 *  Notes on the return values of callbacks:
 *  - on_connect:
 *    - zero: Accept client and start listening for data.
 *    - non-zero: Reject client, e.g. kill connection.
 *  - on_receive:
 *    - zero: Data accepted, continue to listen for further data.
 *    - non-zero: Data rejected, kill connection.
 *  - on_disconnect:
 *    - any: Ignored as client is to be killed anyways.
 */
struct ws_settings {
   enum ws_port port; ///< Port number
   int timeout;
   size_t maxdatasize;
   ws_nodata_cb on_connect;
   ws_data_cb   on_receive;
   ws_nodata_cb on_disconnect;
   void *ws_ctx;
};

/// Default settings for webserver
/**
 *  Use this as:
 *  \code
 *  struct ws_settings *settings = WS_SETTINGS_DEFAULT;
 *  \endcode
 */
#define WS_SETTINGS_DEFAULT { \
   .port = WS_PORT_HTTP, \
   .timeout = 15, \
   .maxdatasize = 1024, \
   .on_connect = NULL, \
   .on_receive = NULL, \
   .on_disconnect = NULL, \
   .ws_ctx = NULL }

// Webserver functions
struct ws *ws_create(struct ws_settings *settings, struct ev_loop *loop);
void ws_destroy(struct ws *instance);
int ws_start(struct ws *instance);
void ws_stop(struct ws *instance);
void ws_print(struct ws *ws);

// Client functions
void ws_conn_kill(struct ws_conn *conn);
void ws_conn_close(struct ws_conn *conn);
int ws_conn_sendf(struct ws_conn *conn, const char *fmt, ...);
int ws_conn_vsendf(struct ws_conn *conn, const char *fmt, va_list arg);
const char *ws_conn_get_ip(struct ws_conn *conn);
void ws_conn_keep_open(struct ws_conn *conn);
void ws_conn_print(struct ws_conn *conn);

#endif

