#include "Hex.h"
#include "sdl.h"

namespace T3E
{
    Hex::Hex() :
        node_( nullptr ),
        type_( NodeType::EMPTY ),
        hexSize_( 0.54f )
    {
    }

    Hex::~Hex()
    {
        delete node_;
    }

    void Hex::init( int row, int col )
    {
        row_ = row;
        col_ = col;
        
        // calculate world x and y coordinates of hex centre, using pointy top layout
		layout_         = glm::mat2( sqrt(3.0f), sqrt(3.0f) / 2.0f, 0.0f, 3.0f / 2.0f );
        layoutInverse_  = glm::mat2( sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f );
		x_ = (layout_[0][0] * col_ + layout_[0][1] * row_) * hexSize_;
		y_ = (layout_[1][0] * col_ + layout_[1][1] * row_) * hexSize_;
    }
}
