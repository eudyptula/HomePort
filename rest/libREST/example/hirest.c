// hirest.c

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

#include "libREST.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ev.h>

// libREST instance
static struct lr *rest = NULL;

int testCB(void *srv_data, void **req_data,
           struct lr_request *req,
           const char* url, size_t url_len)
{
   // TODO This is not a good example
	printf("CB called!!!\n");

   lr_send_start(req, WS_HTTP_500, NULL);
   lr_send_chunkf(req, "Hello ");
   lr_send_chunkf(req, "world");
   lr_send_stop(req);

	return 0;
}

// Handle correct exiting
static void exit_handler(int sig)
{
   // Stop server
   if (rest) {
      lr_stop(rest);
      lr_destroy(rest);
   }

   // Exit
   printf("Exiting....\n");
   exit(sig);
}

int main(int argc, char *argv[])
{
	struct ev_loop *loop = EV_DEFAULT;

	struct lr_settings set = LR_SETTINGS_DEFAULT;
	set.port = WS_PORT_HTTP_ALT;

	#ifdef DEBUG
	printf("Debugging is set\n");
	#endif

	// Register signals for correctly exiting
   signal(SIGINT, exit_handler);
   signal(SIGTERM, exit_handler);

   // Create server
   rest = lr_create(&set, loop);

    // register service
   lr_register_service(rest, "/device/a", testCB, NULL, NULL, NULL,
         NULL, NULL);

   lr_register_service(rest, "/device/b", NULL, testCB, NULL, NULL,
         NULL, NULL);
   lr_register_service(rest, "/device/b", NULL, testCB, NULL, NULL,
         NULL, NULL);
   lr_unregister_service(rest, "/device/b");

   if(!lr_start(rest))
   	ev_run(loop, 0);

	exit_handler(0);
	return 0;
}







