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

#include "graph.h"

/**
 * @brief Graph is a list of unsorted nodes
 */
struct Graph {
    // a graph is a hashtable of GraphNode
    zhash_t *nodes;
};

/**
 * @brief Allocate a new GraphArc structure.
 * @param from A source node
 * @param to A destination node
 * @return A pointer to an allocated GraphArc, or NULL if an error occured.
 */
static GraphArc *graphArcNew(GraphNode *from, GraphNode *to);

/**
 * @brief Initialize an allocated GraphArc structure.
 * @param self An allocated GraphArc to initialize.
 * @param from A source node
 * @param to A destination node
 * @return true on success, false otherwise.
 */
static bool graphArcInit(GraphArc *self, GraphNode *from, GraphNode *to);

Graph *graphNew(void) {
    Graph *self;

    if ((self = calloc(1, sizeof(Graph))) == NULL) {
        return NULL;
    }

    if (!graphInit(self)) {
        graphDestroy(&self);
        error("Graph failed to initialize.");
        return NULL;
    }

    return self;
}

bool graphInit(Graph *self) {
    if (!(self->nodes = zhash_new())) {
        error("Cannot allocate a new hashtable for nodes.");
        return false;
    }

    return true;
}

static GraphArc *graphArcNew(GraphNode *from, GraphNode *to) {
    GraphArc *self;

    if ((self = calloc(1, sizeof(GraphArc))) == NULL) {
        return NULL;
    }

    if (!graphArcInit(self, from, to)) {
        graphArcDestroy(&self);
        error("GraphArc failed to initialize.");
        return NULL;
    }

    return self;
}

static bool graphArcInit(GraphArc *self, GraphNode *from, GraphNode *to) {
    self->from = from;
    self->to = to;
    self->user_data = NULL;

    // add the current arc to the arcs list : from -> self
    zlist_append(self->from->arcs, self);

    return true;
}

GraphNode *graphNodeNew(char *hashKey, void *user_data) {
    GraphNode *self;

    if ((self = calloc(1, sizeof(GraphNode))) == NULL) {
        return NULL;
    }

    if (!graphNodeInit(self, hashKey, user_data)) {
        graphNodeDestroy(&self);
        error("GraphNode failed to initialize.");
        return NULL;
    }

    return self;
}

bool graphNodeInit(GraphNode *self, char *hashKey, void *user_data) {
    self->user_data = user_data;

    if (!(self->arcs = zlist_new())) {
        error("Cannot allocate a new zlist for arcs.");
        return false;
    }

    self->key = strdup(hashKey);

    return true;
}

bool graphLink(Graph *self, GraphNode *node1, GraphNode *node2) {

    if (!(graphAddArc(self, node1, node2))) {
        error("Cannot link node 1 to node 2.");
        return false;
    }
    if (!(graphAddArc(self, node2, node1))) {
        error("Cannot link node 2 to node 1.");
        return false;
    }

    return true;
}

bool graphUnlink(Graph *self, GraphNode *node1, GraphNode *node2) {
    GraphArc *arc1 = NULL;
    GraphArc *arc2 = NULL;

    if (!(arc1 = graphNodeGetArc(node1, node2))) {
        error("Cannot find link between %s and %s.", node1->key, node2->key);
        return false;
    }
    zlist_remove(node1->arcs, arc1);

    if (!(arc2 = graphNodeGetArc(node2, node1))) {
        error("Cannot find link between %s and %s.", node2->key, node1->key);
        return false;
    }
    zlist_remove(node2->arcs, arc2);

    graphArcDestroy(&arc1);
    graphArcDestroy(&arc2);

    return true;
}

GraphArc *graphNodeGetArc(GraphNode *from, GraphNode *to) {
    for (GraphArc *link = zlist_first(from->arcs); link != NULL; link = zlist_next(from->arcs)) {
        if (link->to == to) {
            return link;
        }
    }

    return NULL;
}

bool graphRemoveNode(Graph *self, GraphNode *node)
{
    GraphArc *arc;

    // Disconnect the node from all other nodes
    while ((arc = zlist_first(node->arcs)) != NULL) {
        GraphNode *to = arc->to;
        if (!(graphUnlink(self, node, to))) {
            error ("Cannot unlink %s and %s.", node->key, to->key);
            return false;
        }
    }

    return true;
}

GraphArc *graphAddArc(Graph *self, GraphNode *from, GraphNode *to) {
    bool status = true;
    GraphArc *arc = NULL;

    if (!(arc = graphArcNew(from, to))) {
        error("Cannot create new arc.");
        return NULL;
    }

    if (!zhash_lookup(self->nodes, from->key)) {
        if (zhash_insert(self->nodes, from->key, from) != 0) {
            error ("Cannot insert 'from' node.");
            status = false;
            goto cleanup;
        }
    }

    if (!zhash_lookup(self->nodes, to->key)) {
        if (zhash_insert(self->nodes, to->key, to) != 0) {
            error ("Cannot insert 'to' node.");
            status = false;
            goto cleanup;
        }
    }

cleanup:
    if (!status) {
        free(arc);
        arc = NULL;
    }

    return arc;
}

GraphNode *graphGetNode(Graph *self, char *key) {
    return zhash_lookup(self->nodes, key);
}

bool graphInsertNode(Graph *self, GraphNode *node) {
    return zhash_insert(self->nodes, node->key, node) == 0;
}

char *graphNodeGenKey(GraphNode *self) {
    char *key = NULL;
    size_t keySize = (sizeof(GraphNode *) * 2) + 1;

    if (!(key = calloc(1, keySize))) {
        error("Cannot allocate a node key.");
        return NULL;
    }

    // key = pointer
    snprintf(key, keySize, "%p", self);

    return key;
}

void graphDump(Graph *self) {
    int loopCounter = 0;
    (void)loopCounter;

    for (GraphNode *node = zhash_first(self->nodes); node != NULL; node = zhash_next(self->nodes)) {
        dbg("- Arcs of nodes %c :", 'A' + loopCounter++);
        for (GraphArc *arc = zlist_first(node->arcs); arc != NULL; arc = zlist_next(node->arcs)) {
            dbg("   %s -> %s", arc->from->key, arc->to->key);
        }
    }
}

void graphNodeFree(GraphNode *self) {
    // TODO
}

void graphNodeDestroy(GraphNode **_self) {
    GraphNode *self = *_self;

    if (self) {
        graphNodeFree(self);
        free(self);
    }

    *_self = NULL;
}

void graphArcFree(GraphArc *self) {
    // TODO
}

void graphArcDestroy(GraphArc **_self) {
    GraphArc *self = *_self;

    if (self) {
        graphArcFree(self);
        free(self);
    }

    *_self = NULL;
}

void graphFree(Graph *self) {
    // TODO
}

void graphDestroy(Graph **_self) {
    Graph *self = *_self;

    if (self) {
        graphFree(self);
        free(self);
    }

    *_self = NULL;
}
