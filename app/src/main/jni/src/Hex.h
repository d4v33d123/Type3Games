#ifndef HEX_H
#define HEX_H

#include "glm/glm.hpp"
#include <vector>
#include "Node.h"

#define CHUNK_WIDTH 16

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

    /*
	class Hex
	{
	public:
		static constexpr float HEX_SIZE = 0.54;//lenght of 1 side, gl world units
		//hold neighbor coordinates
		struct neighbor
		{
			int col;
			int row;
			
			neighbor(): col(-1), row(-1)
			{}
		};
		//content
		enum type {STEM_CELL = 0, NORMAL_CELL, MUTATED_CELL, CANCEROUS_CELL, DYING_CELL, DEAD_CELL, BLOOD_VESSEL};
		
		//set coordinates
		virtual void init(int column, int row, int gridColumns, int gridRows);
		bool inRange(float x, float y, float rangeSquared);

		//Setters
		virtual void setType(type t, int parentDeathChance = 0){type_ = t;};
		
		//getters
		float getX(){return x_;};
		float getY(){return y_;};
		float getR(){return r_;};
		float getC(){return c_;};
		const std::vector<neighbor>& getNeighbors() const {return neighbors_;};
		type getType(){return type_;};
	
	protected:
		type type_;//see enum type
		float c_, r_;//grid coords
		float x_, y_;//world coords
		float hexSize_;//half width, gl world units
		glm::mat2 layout_, layoutInverse_;//pointy top hex grid base vectors
		std::vector<neighbor> neighbors_;//row and column coords of neighbors
	};
    */
}

#endif
