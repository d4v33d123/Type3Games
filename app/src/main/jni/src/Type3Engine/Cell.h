#ifndef CELL_H
#define CELL_H

#include "glm/glm.hpp"
#include <vector>
//maybe make a separate class "hex", keep gameplay info here, put grid structure info there...

namespace T3E
{
	class Cell
	{
		
	struct neighbor
	{
		int col;
		int row;
		
		neighbor(): col(-1), row(-1)
		{}
	};
		
	public:
		enum type {NORMAL = 0, STEM, MUTATED, CANCEROUS, DEAD, DYING, BLOOD};
		Cell(float column, float row, type t);
		bool update(float dTime);
	
	
	
		
		//public for now, fuck it
		std::vector<neighbor> neighbors;
		float divTime;
		
		//getters
		float getX(){return x_;};
		float getY(){return y_;};
		float getR(){return r_;};
		float getC(){return c_;};
		int getDeathChance(){return deathChance_;};
		type getType(){return type_;};
		glm::vec4 getTint(){return tint_;};
		
		//setters
		void makeBlue(){tint_ = glm::vec4(0.0f, 0.6f, 1.0f, 1.0f);};//testing purpose
		void setType(type t){type_ = t;};
		void setDeathChance(int dc){deathChance_ = dc;};
		void setTint(glm::vec4 tint){tint_ = tint;};
		
	private:
		float c_, r_;//grid coords
		float x_, y_;//world coords
		type type_;
		glm::vec4 tint_;
		int deathChance_;
		float splitTimer_;
		
		float hexSize_;
		glm::mat2 layout_, layoutInverse_;//pointy top hex grid base vectors
	};
}

#endif