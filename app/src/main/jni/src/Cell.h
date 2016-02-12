#ifndef CELL_H
#define CELL_H

#include "Hex.h"
#include <cstdlib>//rand()

namespace T3E
{
	class Cell: public Node
	{
	public:
        Cell();
        virtual ~Cell();

        /*
		//FOR TESTING STUFF
		void makeGreen(){tint_ = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);};
		void makeYellow(){tint_ = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);};
		
		//max and min split time
		static const int MIN_ST = 50;
		static const int MAX_ST = 500;
				
		virtual void init(int column, int row, int gridColumns, int gridRows);//create dead cell
		bool update(float dTime);//increase timer, ret true if time to split
		void newSplitTime(int min = MIN_ST, int max = MAX_ST);//get a new random div time <= max
		virtual void setType(type t, int parentDeathChance = 0);//set members according to new type, parent
		void incDeathChance(int dc){deathChance_ += dc;};//increase death chance by dc
		
		//Setters
		void setDeathChance(int dc){deathChance_ = dc;};
		
		//getters
		int getDeathChance(){return deathChance_;};
		glm::vec4 getTint(){return tint_;};
			
	private:
		glm::vec4 tint_;//colour for shader
		int deathChance_;//chance to die instead of splitting
		float splitTimer_;//time since last split; milliseconds
		float splitTime_;//time to reach to start split; milliseconds
        */
	};
}

#endif
