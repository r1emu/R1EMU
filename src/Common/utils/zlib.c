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
#include "zlib.h"


// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------
bool
Zlib_compress (
    Zlib *self,
    void *data,
    size_t dataSize
) {
    z_stream stream;

    stream.next_in = data;
    stream.avail_in = dataSize;
    stream.avail_out = sizeof (self->buffer);
    stream.next_out = self->buffer;
    stream.total_in = 0;
    stream.total_out = 0;
    stream.zalloc = 0;
    stream.zfree = 0;

    if (deflateInit2_ (&stream, 1, Z_DEFLATED, -15, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY, "1.2.8", 88)) {
        warning ("Can't init compression.");
        return false;
    }

    if (deflate (&stream, Z_FINISH) != 1) {
        warning ("Can't compress.");
        return false;
    }

    if (deflateEnd (&stream)) {
        warning ("Can't end compression.");
        return false;
    }

    self->header.magic = ZLIB_MAGIC_HEADER;
    self->header.size = stream.total_out;

    return true;
}

bool
Zlib_decompress (
    Zlib *self,
    void *data,
    size_t dataSize
) {
    z_stream stream;

    stream.next_in = data;
    stream.avail_in = dataSize;
    stream.avail_out = sizeof (self->buffer);
    stream.next_out = self->buffer;
    stream.total_in = 0;
    stream.total_out = 0;
    stream.zalloc = 0;
    stream.zfree = 0;

    if (inflateInit2_(&stream, -15, "1.2.8", 88)) {
        buffer_print (data, dataSize, "Buffer : ");
        error ("Can't init decompression.");
        return false;
    }

    if (inflate(&stream, Z_FINISH) != 1) {
        buffer_print (data, dataSize, "Buffer : ");
        error ("Can't decompress.");
        return false;
    }

    if (inflateEnd(&stream)) {
        buffer_print (data, dataSize, "Buffer : ");
        error ("Can't end decompression.");
        return false;
    }

    self->header.magic = ZLIB_MAGIC_HEADER;
    self->header.size = stream.total_out;

    return true;
}
