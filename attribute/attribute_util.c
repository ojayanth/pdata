/* Copyright 2019 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "attribute.h"
#include "attribute_util.h"

struct {
	enum attr_type type;
	char *label;
} attr_type_map[] = {
	{ ATTR_TYPE_UINT8,  "uint8" },
	{ ATTR_TYPE_UINT16, "uint16" },
	{ ATTR_TYPE_UINT32, "uint32" },
	{ ATTR_TYPE_UINT64, "uint64" },
	{ ATTR_TYPE_INT8,   "int8" },
	{ ATTR_TYPE_INT16,  "int16" },
	{ ATTR_TYPE_INT32,  "int32" },
	{ ATTR_TYPE_INT64,  "int64" },
	{ ATTR_TYPE_UNKNOWN, NULL },
};

enum attr_type attr_type_from_string(char *str)
{
	int i;

	for (i=0; attr_type_map[i].label; i++) {
		if (strcmp(str, attr_type_map[i].label) == 0)
			return attr_type_map[i].type;
	}

	return ATTR_TYPE_UNKNOWN;
}

const char *attr_type_to_string(uint8_t type)
{
	int i;

	for (i=0; attr_type_map[i].label; i++) {
		if (type == attr_type_map[i].type)
			return attr_type_map[i].label;
	}

	return "<NULL>";
}

int attr_type_size(enum attr_type type)
{
	int size = 0;

	if (type == ATTR_TYPE_UINT8 || type == ATTR_TYPE_INT8) {
		size = 1;
	} else if (type == ATTR_TYPE_UINT16 || type == ATTR_TYPE_INT16) {
		size = 2;
	} else if (type == ATTR_TYPE_UINT32 || type == ATTR_TYPE_INT32) {
		size = 4;
	} else if (type == ATTR_TYPE_UINT64 || type == ATTR_TYPE_INT64) {
		size = 8;
	} else {
		assert(0);
	}

	return size;
}

void attr_copy(struct attr *src, struct attr *dst)
{
	assert(dst);

	*dst = (struct attr) {
		.type = src->type,
		.data_size = src->data_size,
		.dim_count = src->dim_count,
		.size = src->size,
		.enum_count = src->enum_count,
		.aenum = src->aenum,
	};

	strncpy(dst->name, src->name, ATTR_MAX_LEN);
	if (dst->dim_count > 0) {
		int i;

		dst->dim = malloc(dst->dim_count * sizeof(int));
		assert(dst->dim);

		for (i=0; i<src->dim_count; i++)
			dst->dim[i] = src->dim[i];
	}

	dst->value = malloc(dst->data_size * dst->size);
	assert(dst->value);
	memcpy(dst->value, src->value, dst->data_size * dst->size);
}

void attr_set_value(char *tok, enum attr_type type, uint8_t *ptr)
{
	unsigned long long int data;

	data = strtoull(tok, NULL, 0);

	if (type == ATTR_TYPE_UINT8 || type == ATTR_TYPE_INT8) {
		uint8_t value = data & 0xff;
		memcpy(ptr, &value, 1);
	} else if (type == ATTR_TYPE_UINT16 || type == ATTR_TYPE_INT16) {
		uint16_t value = data & 0xffff;
		memcpy(ptr, &value, 2);
	} else if (type == ATTR_TYPE_UINT32 || type == ATTR_TYPE_INT32) {
		uint32_t value = data & 0xffffffff;
		memcpy(ptr, &value, 4);
	} else if (type == ATTR_TYPE_UINT64 || type == ATTR_TYPE_INT64) {
		uint64_t value = data;
		memcpy(ptr, &value, 8);
	}
}
