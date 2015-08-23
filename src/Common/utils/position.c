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
#include "position.h"


// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------

bool
Math_isWithin2DManhattanDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (dx < distance && dz < distance);
}

bool
Math_isWithin2DCircleDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (square(dx) + square(dz)) < (square(distance));
}
