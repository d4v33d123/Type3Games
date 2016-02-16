#ifndef NODE_H
#define NODE_H

namespace T3E
{
    enum class NodeType { INVALID, EMPTY, CELL, BLOOD_VESSEL_CORE, BLOOD_VESSEL_EDGE };

    class Node
    {
    public:        
        Node();
        virtual ~Node();
        
    protected:
    };
}

#endif
