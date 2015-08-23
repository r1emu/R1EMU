/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Position.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
// Transform a XYZ position to a XZ position
#define PositionXYZToXZ(xyz) \
    (PositionXZ) {.x = (xyz)->x, .z = (xyz)->z}

// Declare a XYZ local variable
#define PositionXYZ_decl(_x, _y, _z) \
    (PositionXYZ) {.x = _x, .y = _y, .z = _z}

// Declare a XZ local variable
#define PositionXZ_decl(_x, _z) \
    (PositionXZ) {.x = _x, .z = _z}

// ------ Structure declaration -------
typedef struct PositionXYZ PositionXYZ;
typedef struct PositionXZ PositionXZ;
/**
 * @brief PositionXYZ contains basic information about a 3D game position {X, Y, Z}
 */
struct PositionXYZ {
    float x, y, z;
};
/**
 * @brief PositionXZ contains basic information about a 2D game position {X, Z}
 */
struct PositionXZ {
    float x, z;
};

// ----------- Functions ------------
/**
 * @brief Check if 2 points are within a given Manhattan distance
 * @param pos1 : position of the first point
 * @param pos2 : position of the second point
 * @param distance : Distance being checked
 * @return true on success, false otherwise
 * References : http://en.wikipedia.org/wiki/Taxicab_geometry#Circles_in_Taxicab_geometry
 */
bool
Math_isWithin2DManhattanDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
);

/**
 * @brief Check if 2 points are within a given euclidian distance
 * @param x1 y1 : position of the first point
 * @param x2 y2 : position of the second point
 * @param range : Distance being checked
 * @return true on success, false otherwise
 * References : http://en.wikipedia.org/wiki/Euclidean_distance
 */
bool
Math_isWithin2DCircleDistance (
    PositionXZ *pos1,
    PositionXZ *pos2,
    float distance
);
