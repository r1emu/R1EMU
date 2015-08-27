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
#include "utils.h"
#include "string.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

void *dumpToMem (
    char *dump,
    void *memory,
    size_t *memSize
) {
    char buffer[1000];
    char localMem[1000000];
    size_t dumpPos = 0;
    size_t memPos = 0;
    uint8_t *memoryBytes = (uint8_t *) memory;
    bool isLocal;

    if (memory == NULL) {
        memory = localMem;
        memoryBytes = localMem;
        isLocal = true;
    }

    while ((dumpPos = strGetLine (dump, buffer, sizeof(buffer) - 1, dumpPos)) != -1)
    {
        char *strPos = buffer;
        if ((strPos = strstr (buffer, "]  ")) != NULL) {
            strPos += strlen("]  ");
        }
        else if ((strPos = strstr (buffer, "] ")) != NULL) {
            strPos += strlen("] ");
        }

        char *end = strstr (strPos, " | ");
        *end = 0;

        while (strPos < end) {
            char *newPos;
            uint8_t octet = strtoul(strPos, &newPos, 16);
            if (strPos == newPos)
                break;
            memoryBytes[memPos++] = octet;
            strPos = newPos;
        }
    }

    if (!isLocal) {
        if (*memSize != memPos) {
            dbg("Warning : memSize != bytes written (%lu != %lu).", (long unsigned) *memSize, (long unsigned) memPos);
            *memSize = memPos;
        }
    } else {
        memory = malloc (memPos);
        memcpy(memory, localMem, memPos);
        *memSize = memPos;
    }

    return memory;
}

void
compareMem (
    void *_mem1,
    size_t memSize1,
    void *_mem2,
    size_t memSize2
) {
    size_t min = (memSize1 < memSize2) ? memSize1 : memSize2;
    uint8_t *mem1 = _mem1;
    uint8_t *mem2 = _mem2;

    dbg("Compare mem : %p (%d bytes) VS %p (%d bytes)", mem1, memSize1, mem2, memSize2);

    for (int i = 0; i < min; i++) {
        // Hex Dump
        if (mem1[i] == mem2[i]) {
            info_green ("%.02X ", mem1[i]);
        } else {
            info_red ("%.02X ", mem1[i]);
        }

        if (i == min-1) {
            for (int j = 0; j < abs ((min%16) - 16) ; j++) {
                info_white ("   ");
            }
        }

        if (i % 16 == 15 || i == min-1) {
            // ASCII Dump
            info_white ("| ");
            int bytesToDisplay = i % 16;
            for (int j = i-bytesToDisplay; j <= i; j++) {
                if (mem1[j] == mem2[j]) {
                    info_green ("%c", isprint (mem1[j]) ? mem1[j] : '.');
                } else {
                    info_red ("%c", isprint (mem1[j]) ? mem1[j] : '.');
                }
            }
            if (i == min-1) {
                for (int j = 0; j < abs ((min%16) - 16) ; j++) {
                    info_white (" ");
                }
            }
            info_white (" || ");

            // Buffer 2 Hex dump
            for (int j = i-bytesToDisplay; j <= i; j++) {
                if (mem1[j] == mem2[j]) {
                    info_green ("%.02X ", mem2[j]);
                } else {
                    info_red ("%.02X ", mem2[j]);
                }
            }
            if (i == min-1) {
                for (int j = 0; j < abs ((min%16) - 16) ; j++) {
                    info_white ("   ");
                }
            }
            info_white ("| ");
            // Buffer 2 ASCII dump
            for (int j = i-bytesToDisplay; j <= i; j++) {
                if (mem1[j] == mem2[j]) {
                    info_green ("%c", isprint (mem2[j]) ? mem2[j] : '.');
                } else {
                    info_red ("%c", isprint (mem2[j]) ? mem2[j] : '.');
                }
            }

            info_white ("\n");
        }
    }

    info_white ("\n");
}
