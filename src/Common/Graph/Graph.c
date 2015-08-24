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
#include "Graph.h"


// ------ Structure declaration -------
/**
 * @brief Graph is a list of unsorted nodes
 */
struct Graph {
    /** A graph is a hashtable of GraphNode */
    zhash_t *nodes;
};

// ------ Static declaration -------
/**
 * @brief Allocate a new GraphArc structure.
 * @param from A source node
 * @param to A destination node
 * @return A pointer to an allocated GraphArc, or NULL if an error occured.
 */
static GraphArc *
GraphArc_new (
    GraphNode *from,
    GraphNode *to
);

/**
 * @brief Initialize an allocated GraphArc structure.
 * @param self An allocated GraphArc to initialize.
 * @param from A source node
 * @param to A destination node
 * @return true on success, false otherwise.
 */
static bool
GraphArc_init (
    GraphArc *self,
    GraphNode *from,
    GraphNode *to
);

// ------ Extern function implementation -------

Graph *
Graph_new (
    void
) {
    Graph *self;

    if ((self = calloc (1, sizeof (Graph))) == NULL) {
        return NULL;
    }

    if (!Graph_init (self)) {
        Graph_destroy (&self);
        error ("Graph failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Graph_init (
    Graph *self
) {
    if (!(self->nodes = zhash_new ())) {
        error ("Cannot allocate a new hashtable for nodes.");
        return false;
    }

    return true;
}

static GraphArc *
GraphArc_new (
    GraphNode *from,
    GraphNode *to
) {
    GraphArc *self;

    if ((self = calloc (1, sizeof (GraphArc))) == NULL) {
        return NULL;
    }

    if (!GraphArc_init (self, from, to)) {
        GraphArc_destroy (&self);
        error ("GraphArc failed to initialize.");
        return NULL;
    }

    return self;
}

static bool
GraphArc_init (
    GraphArc *self,
    GraphNode *from,
    GraphNode *to
) {
    self->from = from;
    self->to = to;
    self->user_data = NULL;

    // Add the current arc to the arcs list : from -> self
    zlist_append (self->from->arcs, self);

    return true;
}

GraphNode *
GraphNode_new (
    char *hashKey,
    void *user_data
) {
    GraphNode *self;

    if ((self = calloc (1, sizeof (GraphNode))) == NULL) {
        return NULL;
    }

    if (!GraphNode_init (self, hashKey, user_data)) {
        GraphNode_destroy (&self);
        error ("GraphNode failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GraphNode_init (
    GraphNode *self,
    char *hashKey,
    void *user_data
) {
    self->user_data = user_data;

    if (!(self->arcs = zlist_new ())) {
        error ("Cannot allocate a new zlist for arcs.");
        return false;
    }

    self->key = strdup (hashKey);

    return true;
}

bool
Graph_link (
    Graph *self,
    GraphNode *node1,
    GraphNode *node2
) {
    if (!(Graph_addArc (self, node1, node2))) {
        error ("Cannot link node 1 to node 2.");
        return false;
    }
    if (!(Graph_addArc (self, node2, node1))) {
        error ("Cannot link node 2 to node 1.");
        return false;
    }

    return true;
}

bool
Graph_unlink (
    Graph *self,
    GraphNode *node1,
    GraphNode *node2
) {
    GraphArc *arc1 = NULL;
    GraphArc *arc2 = NULL;

    if (!(arc1 = GraphNode_getArc (node1, node2))) {
        error ("Cannot find link between %s and %s.", node1->key, node2->key);
        return false;
    }
    zlist_remove (node1->arcs, arc1);

    if (!(arc2 = GraphNode_getArc (node2, node1))) {
        error ("Cannot find link between %s and %s.", node2->key, node1->key);
        return false;
    }
    zlist_remove (node2->arcs, arc2);

    GraphArc_destroy (&arc1);
    GraphArc_destroy (&arc2);

    return true;
}

GraphArc *
GraphNode_getArc (
    GraphNode *from,
    GraphNode *to
) {
    for (GraphArc *link = zlist_first (from->arcs); link != NULL; link = zlist_next (from->arcs)) {
        if (link->to == to) {
            return link;
        }
    }

    return NULL;
}

GraphArc *
Graph_addArc (
    Graph *self,
    GraphNode *from,
    GraphNode *to
) {
    GraphArc *arc = NULL;

    if (!(arc = GraphArc_new (from, to))) {
        error ("Cannot create new arc.");
        return NULL;
    }

    if (!zhash_lookup (self->nodes, from->key)) {
        zhash_insert (self->nodes, from->key, from);
    }

    if (!zhash_lookup (self->nodes, to->key)) {
        zhash_insert (self->nodes, to->key, to);
    }

    return arc;
}

GraphNode *
Graph_getNode (
    Graph *self,
    char *key
) {
    return zhash_lookup (self->nodes, key);
}

bool
Graph_insertNode (
    Graph *self,
    GraphNode *node
) {
    return zhash_insert (self->nodes, node->key, node) == 0;
}

char *
GraphNode_genKey (
    GraphNode *self
) {
    char *key = NULL;
    size_t keySize = (sizeof (GraphNode *) * 2) + 1;

    if (!(key = calloc (1, keySize))) {
        error ("Cannot allocate a node key.");
        return NULL;
    }

    // Key = pointer
    snprintf (key, keySize, "%p", self);

    return key;
}

void
Graph_dump (
    Graph *self
) {
    int loopCounter = 0;
    (void) loopCounter;

    for (GraphNode *node = zhash_first (self->nodes); node != NULL; node = zhash_next (self->nodes)) {
        dbg ("- Arcs of nodes %c :", 'A' + loopCounter++);
        for (GraphArc *arc = zlist_first (node->arcs); arc != NULL; arc = zlist_next (node->arcs)) {
            dbg ("   %s -> %s", arc->from->key, arc->to->key);
        }
    }
}

void
GraphNode_free (
    GraphNode *self
) {
    // TODO
}

void
GraphNode_destroy (
    GraphNode **_self
) {
    GraphNode *self = *_self;

    if (self) {
        GraphNode_free (self);
        free (self);
    }

    *_self = NULL;
}

void
GraphArc_free (
    GraphArc *self
) {
    // TODO
}

void
GraphArc_destroy (
    GraphArc **_self
) {
    GraphArc *self = *_self;

    if (self) {
        GraphArc_free (self);
        free (self);
    }

    *_self = NULL;
}

void
Graph_free (
    Graph *self
) {
    // TODO
}

void
Graph_destroy (
    Graph **_self
) {
    Graph *self = *_self;

    if (self) {
        Graph_free (self);
        free (self);
    }

    *_self = NULL;
}
