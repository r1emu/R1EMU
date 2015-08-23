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
#include "Session.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

Session *
Session_new (
    void
) {
    Session *self;

    if ((self = calloc (1, sizeof (Session))) == NULL) {
        return NULL;
    }

    if (!Session_init (self)) {
        Session_destroy (&self);
        error ("Session failed to initialize.");
        return NULL;
    }

    return self;
}


bool
Session_init (
    Session *self
) {

    return true;
}


void
Session_destroy (
    Session **_self
) {
    Session *self = *_self;

    free (self);
    *_self = NULL;
}
