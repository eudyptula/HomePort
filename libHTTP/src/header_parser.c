// header_parser.c

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "header_parser.h"

enum url_parser_state {
	S_FIELD,
	S_VALUE,
	S_COMPLETED
};

struct header_parser_instance {
	struct header_parser_settings *settings;

	int state;

	char* field_buffer;
	int field_buffer_size;

	char* value_buffer;
	int value_buffer_size;
};

void reset_buffers(struct header_parser_instance *instance)
{
		if(instance->field_buffer != NULL) {
			free(instance->field_buffer);
		}

		if(instance->value_buffer != NULL) {
			free(instance->value_buffer);
		}

		instance -> field_buffer_size = 0;
		instance -> field_buffer = NULL;

		instance -> value_buffer_size = 0;
		instance -> value_buffer = NULL;
}

struct header_parser_instance *hp_create(struct header_parser_settings *settings)
{
	struct header_parser_instance *instance;

	instance = malloc(sizeof(struct header_parser_instance));

	instance -> settings = malloc(sizeof(struct header_parser_settings));

	if(instance->settings == NULL)
	{
		fprintf(stderr, "Malloc failed in header parser when allocating space for header parser settings\n");
		return NULL;
	}

	memcpy(instance->settings, settings, sizeof(struct header_parser_settings));

	instance -> state = S_FIELD;

	reset_buffers(instance);

	return instance;
}

void hp_on_header_field(struct header_parser_instance *instance ,char* field_chunk, int length)
{
	if(instance->state == S_COMPLETED) {
		fprintf(stderr, "The header parser received additional data after a call to completed\n");
		return;
	}

	int old_buffer_size = instance->field_buffer_size;

	// If state is S_VALUE, yield a field-value pair
	if(instance->state == S_VALUE) {
		instance->settings->on_field_value_pair(instance->field_buffer, instance->field_buffer_size, instance->value_buffer, instance->value_buffer_size);

		reset_buffers(instance);
	}

	instance->state = S_FIELD;
	instance->field_buffer_size += length;

	instance->field_buffer = realloc(instance->field_buffer, instance->field_buffer_size * (sizeof(char)));
	if(instance->field_buffer == NULL)
	{
		fprintf(stderr, "Realloc failed in URL parser when allocating space for new URL chunk\n");
		return;
	}

	memcpy(instance->field_buffer+old_buffer_size, field_chunk, length);

	return;
}

void hp_on_header_value(struct header_parser_instance *instance, char* value_chunk, int length)
{
	if(instance->state == S_COMPLETED) {
		fprintf(stderr, "The header parser received additional data after a call to completed\n");
		return;
	}

	instance->state = S_VALUE;

	int old_buffer_size = instance->value_buffer_size;

	instance->value_buffer_size += length;

	instance->value_buffer = realloc(instance->value_buffer, instance->value_buffer_size * (sizeof(char)));
	if(instance->value_buffer == NULL)
	{
		fprintf(stderr, "Realloc failed in URL parser when allocating space for new URL chunk\n");
		return;
	}

	memcpy(instance->value_buffer+old_buffer_size, value_chunk, length);

	return;
}

void hp_on_header_complete(struct header_parser_instance *instance)
{
	if(instance->state == S_VALUE)
	{
		instance->settings->on_field_value_pair(instance->field_buffer, instance->field_buffer_size, instance->value_buffer, instance->value_buffer_size);
		instance->state = S_COMPLETED;
	} else if (instance->state == S_FIELD) {
		fprintf(stderr, "The header parser was missing a value when the call to completed was made\n");
	}
	else
	{
		// Completed called multiple times
	}
	
	return;
}

void hp_destroy(struct header_parser_instance *instance)
{
	if(instance != NULL){ 

		if(instance->settings != NULL) {
			free(instance->settings);
		}

		reset_buffers(instance);
	
		free(instance);
	}
}