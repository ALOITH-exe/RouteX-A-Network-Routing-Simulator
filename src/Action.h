#ifndef ACTION_H
#define ACTION_H

enum class ActionType {
    AddNode,
    AddEdge,
    SelectNode
};

struct Action {
    ActionType type;
    int nodeId;   // for AddNode
    int u, v;     // for AddEdge
};

#endif
