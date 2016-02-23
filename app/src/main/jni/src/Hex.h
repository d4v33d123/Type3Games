#ifndef HEX_H
#define HEX_H

#include "glm/glm.hpp"
#include <vector>
#include "Node.h"

#define CHUNK_WIDTH 48

namespace T3E
{
    class Hex
    {
    public:
        
        // Hex's are initialise to empty
        // that is Node* == nullptr and NodeType == NodeType::EMPTY

        Hex();
        ~Hex();

        void init( int row, int col );
			
        // Getters
        inline int getRow() const { return row_; }
        inline int getCol() const { return col_; }
        inline float getX() const { return x_; }
        inline float getY() const { return y_; }
        inline Node* getNode() const { return node_; }
        inline NodeType getType() const { return type_; }
		
        // Setters
        inline void setNode( Node* node ) { node_ = node; }
        inline void setType( NodeType type ) { type_ = type; }

    private:
        
        Node* node_;    // Contains the functionality of the hex
        NodeType type_; // The type of the hex's node
        int row_, col_; // Grid coords
        float x_, y_;   // World coords
        float hexSize_; // Half width in OpenGL units TODO: this could be a #define instead? or constexpr
        glm::mat2 layout_, layoutInverse_; // Point top hex grid base vectors        
    };

    // Hex's are considered the same if they are in the same position, NOTE: their actual content is being ignored!
    //inline bool operator == ( const Hex& lhs, const Hex& rhs ) { return ( lhs.getRow() == rhs.getRow() ) && ( lhs.getCol() == rhs.getCol() ); }
}

#endif
