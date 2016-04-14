/*
 * Copyright 2011 Aalborg University. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * THIS SOFTWARE IS PROVidED BY Aalborg University ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Aalborg University OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 */

/**
 * @file hpd_services.c
 * @brief  Methods for managing the service_t structure
 * @author Thibaut Le Guilly
 * @author Regis Louge
 */

#include "datamanager.h"
#include "hp_macros.h"
#include "utlist.h"
#include "hpd_error.h"

/**
 * Frees all the memory allocated for the service_t. Note
 * that it only frees the memory used by the API, if the
 * user allocates memory for service_ts attributes, he needs
 * to free it before/after calling this function. Also note
 * that the user can't destroy a service_t that is
 * registered on the server.
 *
 * @param service_to_destroy The service to destroy
 *
 * @return returns A HPD error code
 */
void
serviceFree( service_t *service )
{

  if( service != NULL )
  {
     deviceRemoveService(service);
    free_pointer(service->description);
    free_pointer(service->type);
    free_pointer(service->unit);
    free_pointer(service->id);
    parameterFree(service->parameter);
    if (service->free_data) service->free_data(service->data);
    free(service);
  }
}

int
serviceAddListener(service_t *service, listener_t *l)
{
   if( service == NULL || l == NULL ) 
      return HPD_E_NULL_POINTER;
   
   DL_APPEND( service->listener_head, l);
   return HPD_E_SUCCESS;
}

int 
serviceRemoveListener(service_t *service, listener_t *l)
{
   if( service == NULL || l == NULL ) return HPD_E_NULL_POINTER;
   DL_DELETE( service->listener_head, l );
   return HPD_E_SUCCESS; 
}

