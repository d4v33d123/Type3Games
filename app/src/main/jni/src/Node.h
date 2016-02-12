#ifndef NODE_H
#define NODE_H

namespace T3E
{
    enum class NodeType { INVALID, EMPTY, CELL, BLOOD_VESSEL };

    class Node
    {
    public:        
        Node();
        virtual ~Node();
        
    protected:
    };
}

#endif
