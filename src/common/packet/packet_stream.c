/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "packet_stream.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation ------

extern inline void packetStreamResetPosition(PacketStream *self);

PacketStream *packetStreamNew(void *buffer, size_t size) {
    PacketStream *self;

    if ((self = malloc(sizeof(PacketStream))) == NULL) {
        error("Cannot allocate enough space.");
        return NULL;
    }

    packetStreamInit(self, buffer, size);

    return self;
}

void packetStreamInit(PacketStream *self, void *buffer, size_t size) {
    memset(self, 0, sizeof(*self));
    self->buffer = buffer;
    self->position = 0;
    self->size = size;
    self->sizeExcepted = 0;
    self->function = NULL;
}

void packetStreamAppend(PacketStream *self, void *data, size_t dataSize) {

    if (self->position + dataSize > self->size) {
        error("Error, cannot get data from the stream in '%s' because it exceeds the stream size '%d' ('%d' requested).",
              self->function, self->size, self->position + dataSize);
        return;
    }

    memcpy(&self->buffer[self->position], data, dataSize);
    self->position += dataSize;
    self->sizeExcepted -= dataSize;
}

void *packetStreamGetCurrentBuffer(PacketStream *self) {
    return &self->buffer[self->position];
}

void *packetStreamGetBuffer(PacketStream *self) {
    return self->buffer;
}

void packetStreamAddOffset(PacketStream *self, unsigned int offset) {
    self->position += offset;
}

void packetStreamGet(PacketStream *self, void *data, size_t dataSize) {

    if (self->position + dataSize > self->size) {
        error("Error, cannot get data from the stream in '%s' because it exceeds the stream size '%d' ('%d' requested).",
              self->function, self->size, self->position + dataSize);
        return;
    }

    memcpy(data, &self->buffer[self->position], dataSize);
    self->position += dataSize;
    self->sizeExcepted -= dataSize;
}

void packetStreamDestroy(PacketStream **_self) {
    PacketStream *self = *_self;

    free(self);
    *_self = NULL;
}

size_t packetStreamGetSize(PacketStream *self) {
    return self->position;
}

void packetStreamPrint(PacketStream *self) {
    buffer_print(self->buffer, self->position, "PacketStream : ");
}

void _packetStreamDebugStart(PacketStream *self, size_t sizeExcepted, const char *function) {
    self->sizeExcepted = sizeExcepted;
    self->function = function;
}

bool _packetStreamDebugEnd(PacketStream *self, const char *function) {
    if (self->sizeExcepted != 0) {
        error("PacketStream didn't write the expected size in '%s'. Offset = %d", function, self->sizeExcepted);
        return false;
    }

    return true;
}

