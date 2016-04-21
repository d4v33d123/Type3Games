#ifndef NODE_H
#define NODE_H

namespace T3E
{
    /** NodeType enum
    * Used to distinguish between the different types that inherit from node
    * Including empty nodes, cells and blood vessels
    */
    enum class NodeType { INVALID, EMPTY, CELL, BLOOD_VESSEL_CORE, BLOOD_VESSEL_EDGE };
	
    /**
    * Base class for objects that are to be stored within a hex.
    * @see Cell
    * @see BloodVessel
    */
    class Node
    {
    public:        
        Node();
        virtual ~Node();
		        
    protected:
    };
}

#endif
