#ifndef CELL_H
#define CELL_H

#include "Type3Engine/Hex.h"
#include <cstdlib>//rand()

namespace T3E
{
	class Cell
	{
	public:
		void makePink(){tint_ = glm::vec4(1.0f, 0.8f, 0.8f, 1.0f);};//FOR TESTING STUFF
		
		//max and min split time
		static const int MIN_ST = 50;
		static const int MAX_ST = 500;
		
		static constexpr float HEX_SIZE = 0.54;//half width, gl world units
		enum type {NORMAL = 0, STEM, MUTATED, CANCEROUS, DEAD, DYING};//cell types
		
		Cell(int column, int row, int gridColumns, int gridRows);//create dead cell
		bool update(float dTime);//increase timer, ret true if time to split
		void newSplitTime(int min = MIN_ST, int max = MAX_ST);//get a new random div time <= max
		void changeType(type t, int parentDeathChance = 0);//set members according to new type, parent
		void incDeathChance(int dc){deathChance_ += dc;};//increase death chance by dc
		
		//getters
		Hex& hex(){return hex_;};
		int getDeathChance(){return deathChance_;};
		type getType(){return type_;};
		glm::vec4 getTint(){return tint_;};
			
	private:
		Hex hex_;//hex in which the cell is
		type type_;//see enum type
		glm::vec4 tint_;//colour for shader
		int deathChance_;//chance to die instead of splitting
		float splitTimer_;//time since last split; milliseconds
		float splitTime_;//time to reach to start split; milliseconds
	};
}

#endif