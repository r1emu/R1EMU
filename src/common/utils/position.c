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
#include "position.h"


// ------ Structure declaration -------

// ------ Static declaration -------


// ------ Extern function implementation -------

bool
mathIsWithin2DManhattanDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (dx < distance && dz < distance);
}

bool
mathIsWithin2DCircleDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
) {
    float dx = abs (pos2->x - pos1->x);
    float dz = abs (pos2->z - pos1->z);

    return (square(dx) + square(dz)) < (square(distance));
}


void positionXYZDump (PositionXYZ *position) {
    dbg ("x = %f / y = %f / z = %f", position->x, position->y, position->z);
}

void positionXZDump (PositionXZ *position) {
    dbg ("x = %f / z = %f", position->x, position->z);
}
