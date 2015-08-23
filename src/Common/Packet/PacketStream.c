/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */


// ---------- Includes ------------
#include "PacketStream.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation ------

PacketStream *
PacketStream_new (
    uint8_t *buffer
) {
    PacketStream *self;

    if ((self = calloc (1, sizeof (PacketStream))) == NULL) {
        return NULL;
    }

    if (!PacketStream_init (self, buffer)) {
        PacketStream_destroy (&self);
        error ("PacketStream failed to initialize.");
        return NULL;
    }

    return self;
}


bool
PacketStream_init (
    PacketStream *self,
    uint8_t *buffer
) {
    self->buffer = buffer;
    self->position = 0;
    return true;
}

void
PacketStream_append (
    PacketStream *self,
    void *data,
    size_t dataSize
) {
    memcpy (&self->buffer[self->position], data, dataSize);
    self->position += dataSize;
}


void
PacketStream_addOffset (
    PacketStream *self,
    unsigned int offset
) {
    self->position += offset;
}

void
PacketStream_destroy (
    PacketStream **_self
) {
    PacketStream *self = *_self;

    free (self);
    *_self = NULL;
}
