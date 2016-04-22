#ifndef GLOBAL_POINTS
#define GLOBAL_POINTS

namespace T3E
{

	struct SCORE
	{
	public:

		/** @name Score value setters
		* These set the score values awarded to the player for various actions in the game
		*/
		///@{
		static void SET_SPAWNED_HEALTHY_CELL( int score ) { spawned_healthy_cell_ = score; }
		static void SET_SPAWNED_MUTATED_CELL( int score ) { spawned_mutated_cell_ = score; }
		static void SET_SPAWNED_CANCER_CELL( int score ) { spawned_cancer_cell_ = score; }
		static void SET_SPAWNED_BLOODVESSEL( int score ) { spawned_bloodvessel_ = score; }
		static void SET_SPAWNED_STEM_CELL( int score ) { spawned_stem_cell_ = score; }
		static void SET_ARRESTED_CELL( int score ) { arrested_cell_ = score; }

		static void SET_KILLED_HEALTHY_CELL( int score ) { killed_healthy_cell_ = score; }
		static void SET_KILLED_MUTATED_CELL( int score ) { killed_mutated_cell_ = score; }
		static void SET_KILLED_CANCER_CELL( int score ) { killed_cancer_cell_ = score; }
		static void SET_KILLED_BLOODVESSEL( int score ) { killed_bloodvessel_ = score; }
		static void SET_KILLED_STEM_CELL( int score ) { killed_stem_cell_ = score; }
		static void SET_KILLED_ARRESTED_CELL( int score ) { killed_arrested_cell_  = score; }

		static void SET_CANCER_PER_SECOND( int score ) { cancer_per_second_ = score; }
		///@}

		/** @name Score value getters
		* Returns the score values for actions in game
		*/
		///@{
		static int SPAWNED_HEALTHY_CELL() { return spawned_healthy_cell_; }
		static int SPAWNED_MUTATED_CELL() { return spawned_mutated_cell_; }
		static int SPAWNED_CANCER_CELL() { return spawned_cancer_cell_; }
		static int SPAWNED_BLOODVESSEL() { return spawned_bloodvessel_; }
		static int SPAWNED_STEM_CELL() { return spawned_stem_cell_; }
		static int ARRESTED_CELL() { return arrested_cell_; }

		static int KILLED_HEALTHY_CELL() { return killed_healthy_cell_; }
		static int KILLED_MUTATED_CELL() { return killed_mutated_cell_; }
		static int KILLED_CANCER_CELL() { return killed_cancer_cell_; }
		static int KILLED_BLOODVESSEL() { return killed_bloodvessel_; }
		static int KILLED_STEM_CELL() { return killed_stem_cell_; }
		static int KILLED_ARRESTED_CELL() { return killed_arrested_cell_; }

		static int CANCER_PER_SECOND() { return cancer_per_second_; }
		///@}

	private:
		static int spawned_healthy_cell_;
		static int spawned_mutated_cell_;
		static int spawned_cancer_cell_;
		static int spawned_bloodvessel_;
		static int spawned_stem_cell_;
		static int arrested_cell_;

		static int killed_healthy_cell_;
		static int killed_mutated_cell_;
		static int killed_cancer_cell_;
		static int killed_bloodvessel_;
		static int killed_stem_cell_;
		static int killed_arrested_cell_;

		static int cancer_per_second_;
	};

}

#endif