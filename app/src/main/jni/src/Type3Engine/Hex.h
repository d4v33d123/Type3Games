#ifndef HEX_H
#define HEX_H

#include "glm/glm.hpp"
#include <vector>

namespace T3E
{
	class Hex
	{
	public:
		struct neighbor
		{
			int col;
			int row;
			
			neighbor(): col(-1), row(-1)
			{}
		};
		void init(int column, int row, int gridColumns, int gridRows, float hexSize);	
			
		//getters
		float getX(){return x_;};
		float getY(){return y_;};
		float getR(){return r_;};
		float getC(){return c_;};
		const std::vector<neighbor>& getNeighbors() const {return neighbors_;};
		
	private:
		float c_, r_;//grid coords
		float x_, y_;//world coords
		float hexSize_;//half width, gl world units
		glm::mat2 layout_, layoutInverse_;//pointy top hex grid base vectors
		std::vector<neighbor> neighbors_;//row and column coords of neighbors
	};
}

#endif