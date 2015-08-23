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
#include "R1EMU.h"
#include <sys/time.h>

// ------ Structure declaration -------


// ------ Static declaration -------

// http://www.concentric.net/~Ttwang/tech/inthash.htm
static uint32_t mix (uint32_t a, uint32_t b, uint32_t c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}


// ------ Extern function implementation
int
rand_r (
    unsigned int *seed
) {
    // http://fossies.org/dox/glibc-2.21/rand__r_8c_source.html
    unsigned int next = *seed;
    int result;
    next *= 1103515245;
    next += 12345;
    result = (unsigned int) (next / 65536) % 2048;
    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;
    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;
    *seed = next;
    return result;
}

uint32_t
R1EMU_seed_random (
    uint32_t customData
) {
    int pid, tid;
    #ifdef WIN32
        pid = (int) GetCurrentProcessId ();
        tid = (int) GetCurrentThreadId ();
    #else
        pid = (int) getpid ();
        tid = (int) pthread_self ();
    #endif

    struct timeval tv;
    gettimeofday (&tv, NULL);

    uint32_t seed = mix (clock(), tv.tv_usec, pid + tid + customData);

    return seed;
}

uint32_t
R1EMU_generate_random (
    uint32_t *seed
) {
    return rand_r (seed);
}

uint64_t
R1EMU_generate_random64 (
    uint32_t *seed
) {
    uint32_t low  = R1EMU_generate_random (seed);
    uint64_t high = R1EMU_generate_random (seed);
    high <<= (sizeof (uint32_t) * 8 - 1);
    return high | low;
}
