#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

class GameObject
{
public:
	enum type {STEM_CELL = 0, NORMAL_CELL, MUTATED_CELL, CANCER_CELL, DYING_CELL, DEAD_CELL, BLOOD_VESSEL_CORE, BLOOD_VESSEL_PART};
	
	void setType(type t){type_ = t;};
	type getType(){return type_;};
}

#endif