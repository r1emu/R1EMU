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
zlibCompress (
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

    int result;
    if ((result = deflateInit2 (&stream, 1, Z_DEFLATED, -15, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)) != Z_OK) {
        warning ("Can't init compression : error code = %x.", result);
        return false;
    }

    if ((result = deflate (&stream, Z_FINISH) != Z_STREAM_END)) {
        warning ("Can't compress : error code = %x.", result);
        return false;
    }

    if ((result = deflateEnd (&stream) != Z_OK)) {
        warning ("Can't end compression : error code = %x", result);
        return false;
    }

    self->header.magic = ZLIB_MAGIC_HEADER;
    self->header.size = stream.total_out;

    return true;
}

bool
zlibDecompress (
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
