#include "buffer.h"

buffer_t *buffer_create(void) {
  buffer_t *buffer = malloc(sizeof(buffer_t));
  if (buffer == NULL)
    return NULL;
  buffer->size = 0;
  buffer->offset = 0;
  buffer->stream = NULL;
  return buffer;
}

void buffer_destroy(buffer_t *buffer) {
  free(buffer->stream);
  free(buffer);
}

int buffer_add(buffer_t *buffer, void *data, size_t size) {
  if (buffer->stream == NULL) {
    buffer->stream = malloc(size);
    if (buffer->stream == NULL)
      return -1;
  } else {
    buffer->stream = realloc(buffer->stream, buffer->size + size);
    if (buffer->stream == NULL)
      return -1;
  }
  memcpy(buffer->stream + buffer->size, data, size);
  buffer->size += size;
  return 0;
}

void buffer_read(buffer_t *buffer, void *data, size_t size) {
  memcpy(data, buffer->stream + buffer->offset, size);
  buffer->offset += size;
}

int buffer_add_uint32(buffer_t *buffer, uint32_t data) {
  return buffer_add(buffer, &data, sizeof(uint32_t));
}

int buffer_add_uint8(buffer_t *buffer, uint8_t data) {
  return buffer_add(buffer, &data, sizeof(uint8_t));
}

int buffer_add_string(buffer_t *buffer, uint32_t length, char *string) {
  return buffer_add(buffer, string, sizeof(char) * (length + 1));
}

uint32_t buffer_read_uint32(buffer_t *buffer) {
  uint32_t res;
  buffer_read(buffer, &res, sizeof(uint32_t));
  return res;
}

uint8_t buffer_read_uint8(buffer_t *buffer) {
  uint8_t res;
  buffer_read(buffer, &res, sizeof(uint8_t));
  return res;
}

char *buffer_read_string(buffer_t *buffer, uint32_t length) {
  char *res = malloc(sizeof(char) * length + 1);
  buffer_read(buffer, res, sizeof(char) * (length + 1));
  return res;
}

buffer_t *buffer_dup(buffer_t *buffer) {
  buffer_t *duplicated = buffer_create();
  if (duplicated == NULL)
    return NULL;

  duplicated->size = buffer->size;
  duplicated->offset = buffer->offset;
  duplicated->stream = malloc(duplicated->size);

  if (duplicated->stream == NULL) {
    buffer_destroy(duplicated);
    return NULL;
  }

  memcpy(duplicated->stream, buffer->stream, buffer->size);
  return duplicated;
}
