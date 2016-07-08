#include "MainGame.h"
#include "Type3Engine/ConfigFile.h"
#include "Type3Engine/TextRenderer.h"
#include "Type3Engine/window.h"
#include "GlobalScoreValues.h"

MainGame::MainGame():
	score_(0),
	gameState_(GameState::PLAY),
	maxFPS_(60.0f),
	nOfFingers_(0),
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(4.0f),
    finger_dragged_(false),
    finger_pressed_(false),
	pressTimer_(0),
	finger_down_(false),
	cellSelected_(false),
	interactionMode_(InteractionMode::NORMAL),
	paused_(false),
	gameOver_(false),
	tutorial_(false),
	tut_phase_(TutorialPhase::READY)
{}

MainGame::~MainGame()
{
	//free memory
	for (std::vector<T3E::Sprite*>::iterator it = sprites_.begin() ; it != sprites_.end(); ++it) {
		delete (*it);
	} 
	sprites_.clear();
}

command MainGame::run(T3E::window* window, T3E::AudioEngine* audioEngine, T3E::TextRenderer* textRenderer, bool tutorial)
{
	window_ = window;
	audioEngine_ = audioEngine;
	textRenderer_ = textRenderer;
	tutorial_ = tutorial;
	if( tutorial_ ) {
		grid_.setGridUpdates(false);
	}
	
	initSystems();
	
 	sprites_.push_back( new T3E::Sprite() );
	sprites_.back()->init( -1.5f, -1.5f, 3.0f, 3.0f,"textures/bloodVessel.png", 4/5.0f, 2/5.0f, 1.0f/5, 1.0f/5 );
    
	T3E::Music music		= audioEngine_->loadMusic("sound/backgroundSlow.ogg");
	music.play( -1 );
	
	blood_vessel_sound_		= audioEngine_->loadSoundEffect("sound/Blood_Vessel_placeholder.ogg");
	cell_death_sound_		= audioEngine_->loadSoundEffect("sound/Player_CellDeath.ogg");
	cell_arrest_sound_		= audioEngine_->loadSoundEffect("sound/Player_CellArrest.ogg");
	cell_mode_change_sound_	= audioEngine_->loadSoundEffect("sound/Player_CellModeChange.ogg");
	select_sound_			= audioEngine_->loadSoundEffect("sound/Player_Select.ogg");
	
	return gameLoop();
}

void MainGame::initSystems()
{
	glClearDepthf(1.0);		// Clear the depth buffer to a value of 1 (really far away)
	glLineWidth(5.0f);		// Set lines to render at 5px wide

	/* init projection matrix */
	window_->updateSizeInfo(); // can do just once here since screen orientation is set to landscape always
	float aspect_ratio = float( window_->getScreenWidth() ) / float( window_->getScreenHeight() );
	projectionM_ = glm::perspective( 90.0f, aspect_ratio, 0.1f, 100.0f ); // fov in degrees, aspect ratio, near, and far clipping plane
	/* init ortho matrix */
	// inverting top with bottom to avoid sprites being drawn upside down
	// note that this will put origin at bottom left, while screen coords have origin at top left
	orthoM_ = glm::ortho(0.0f, float( window_->getScreenWidth() ), 0.0f, float( window_->getScreenHeight() ));
	
	// Pass a pointer to the tutorial control
	grid_.setTutorialPhase( &tut_phase_ );

	// Load the config file to get parameters from
	T3E::ConfigFile configFile("config/config.txt");

	float cam_x, cam_y, cam_z;

	// Try to read a value from the config file, otherwise give a default
	if( !configFile.getFloat("cam_x", &cam_x) ) cam_x = 10.0f;
	if( !configFile.getFloat("cam_y", &cam_y) ) cam_y = 10.0f;
	if( !configFile.getFloat("cam_z", &cam_z) ) cam_z = 2.0f;

	// init camera at 0,0,1 looking at origin, up is y axis
	camera_.init( glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f,0.0f,0.0f ) );
	camera_.setZoomRange( glm::vec2(1.0f, 8.0f) );
	camera_.setSensitivity( PAN_SENSITIVITY, ZOOM_SENSITIVITY );
	camera_.moveTo(glm::vec3( cam_x, cam_y, cam_z ) );
	
	int min_split_time, max_split_time, chance_of_mutation, chance_of_cancer;
	int cancer_death_chance, adjacent_bv_death_chance, far_bv_death_chance, child_death_chance_increase, parent_death_chance_increase;
	int min_death_chance, max_death_chance;
	
	// Set the cell properties
	configFile.getInt("cell_min_split_time", 			&min_split_time, 			 500);
	configFile.getInt("cell_max_split_time", 			&max_split_time, 			5000);
	configFile.getInt("chance_of_mutation", 			&chance_of_mutation, 		  50);
	configFile.getInt("chance_of_cancer", 				&chance_of_cancer, 			  50);
	configFile.getInt("cancer_death_chance", 			&cancer_death_chance, 		  50);
	configFile.getInt("adjacent_bv_death_chance", 		&adjacent_bv_death_chance, 	  50);
	configFile.getInt("far_bv_death_chance", 			&far_bv_death_chance, 		  50);
	configFile.getInt("child_death_chance_increase", 	&child_death_chance_increase,  5);
	configFile.getInt("parent_death_chance_increase", 	&parent_death_chance_increase, 5);
	configFile.getInt("min_death_chance", 				&min_death_chance, 			   5);
	configFile.getInt("max_death_chance", 				&max_death_chance, 			  95);
	
	T3E::Cell::MIN_ST = min_split_time;
	T3E::Cell::MAX_ST = max_split_time;
	grid_.setChanceOfMutation( chance_of_mutation );
	grid_.setChanceOfCancer( chance_of_cancer );
	grid_.setCancerDeathChance( cancer_death_chance );
	grid_.setAdjBloodvesselDeathChance( adjacent_bv_death_chance );
	grid_.setFarBloodvesselDeathChance( far_bv_death_chance );
	grid_.setChildDeathChanceIncrease( child_death_chance_increase );
	grid_.setParentDeathChanceIncrease( parent_death_chance_increase );
	grid_.setMinDeathChance( min_death_chance );
	grid_.setMaxDeathChance( max_death_chance );
	
	float bloodvessel_range;

	// Set the bloodvessel properties
	if( !configFile.getFloat("bloodvessel_range", &bloodvessel_range) ) bloodvessel_range = 5.0f;

	T3E::BloodVessel::setRange( bloodvessel_range );

	//windows line endings cause mayhem!!!
 	// configFile.getString( "bloodvessel_button_image",	&bloodvessel_button_image );
	// configFile.getString( "kill_button_image",			&kill_button_image );
	// configFile.getString( "background_image",			&background_image );

	// Get colour ranges from config file
	{
		float r_min, r_max, g_min, g_max, b_min, b_max;

		// Set the colour range of the normal cell
		configFile.getFloat("normal_col_min_red", &r_min);
		configFile.getFloat("normal_col_max_red", &r_max);
		configFile.getFloat("normal_col_min_grn", &g_min);
		configFile.getFloat("normal_col_max_grn", &g_max);
		configFile.getFloat("normal_col_min_blu", &b_min);
		configFile.getFloat("normal_col_max_blu", &b_max);

		T3E::Cell::normalColourRange_[0] = glm::vec4( r_min, g_min, b_min, 255.0f );
		T3E::Cell::normalColourRange_[1] = glm::vec4( r_max, g_max, b_max, 255.0f );		
	}
	{
		float r_min, r_max, g_min, g_max, b_min, b_max;

		// Set the colour range of the mutated cell
		configFile.getFloat("mutated_col_min_red", &r_min);
		configFile.getFloat("mutated_col_max_red", &r_max);
		configFile.getFloat("mutated_col_min_grn", &g_min);
		configFile.getFloat("mutated_col_max_grn", &g_max);
		configFile.getFloat("mutated_col_min_blu", &b_min);
		configFile.getFloat("mutated_col_max_blu", &b_max);

		T3E::Cell::mutatedColourRange_[0] = glm::vec4( r_min, g_min, b_min, 255.0f );
		T3E::Cell::mutatedColourRange_[1] = glm::vec4( r_max, g_max, b_max, 255.0f );		
	}
	{
		float r_min, r_max, g_min, g_max, b_min, b_max;

		// Set the colour range of the cancer cell
		configFile.getFloat("cancer_col_min_red", &r_min);
		configFile.getFloat("cancer_col_max_red", &r_max);
		configFile.getFloat("cancer_col_min_grn", &g_min);
		configFile.getFloat("cancer_col_max_grn", &g_max);
		configFile.getFloat("cancer_col_min_blu", &b_min);
		configFile.getFloat("cancer_col_max_blu", &b_max);

		T3E::Cell::cancerousColourRange_[0] = glm::vec4( r_min, g_min, b_min, 255.0f );
		T3E::Cell::cancerousColourRange_[1] = glm::vec4( r_max, g_max, b_max, 255.0f );		
	}

	// Get the score values from the config file
	{
		int spawned_healthy_cell_, spawned_mutated_cell_, spawned_cancer_cell_, spawned_bloodvessel_, spawned_stem_cell_, arrested_cell_;
		int killed_healthy_cell_, killed_mutated_cell_, killed_cancer_cell_, killed_bloodvessel_, killed_stem_cell_, killed_arrested_cell_;
		int cancer_per_second_;

		configFile.getInt("spawned_healthy_cell", &spawned_healthy_cell_, 1 );
		configFile.getInt("spawned_mutated_cell", &spawned_mutated_cell_, 1 );
		configFile.getInt("spawned_cancer_cell", &spawned_cancer_cell_, 1 );
		configFile.getInt("spawned_bloodvessel", &spawned_bloodvessel_, 1 );
		configFile.getInt("spawned_stem_cell", &spawned_stem_cell_, 1 );
		configFile.getInt("arrested_cell", &arrested_cell_, 1 );

		configFile.getInt("killed_healthy_cell", &killed_healthy_cell_, 1 );
		configFile.getInt("killed_mutated_cell", &killed_mutated_cell_, 1 );
		configFile.getInt("killed_cancer_cell", &killed_cancer_cell_, 1 );
		configFile.getInt("killed_bloodvessel", &killed_bloodvessel_, 1 );
		configFile.getInt("killed_stem_cell", &killed_stem_cell_, 1 );
		configFile.getInt("killed_arrested_cell", &killed_arrested_cell_, 1 );

		configFile.getInt("cancer_per_second", &cancer_per_second_, -1 );

		T3E::SCORE::SET_SPAWNED_HEALTHY_CELL( spawned_healthy_cell_ );
		T3E::SCORE::SET_SPAWNED_MUTATED_CELL( spawned_mutated_cell_ );
		T3E::SCORE::SET_SPAWNED_CANCER_CELL( spawned_cancer_cell_ );
		T3E::SCORE::SET_SPAWNED_BLOODVESSEL( spawned_bloodvessel_ );
		T3E::SCORE::SET_SPAWNED_STEM_CELL( spawned_stem_cell_ );
		T3E::SCORE::SET_ARRESTED_CELL( arrested_cell_ );

		T3E::SCORE::SET_KILLED_HEALTHY_CELL( killed_healthy_cell_ );
		T3E::SCORE::SET_KILLED_MUTATED_CELL( killed_mutated_cell_ );
		T3E::SCORE::SET_KILLED_CANCER_CELL( killed_cancer_cell_ );
		T3E::SCORE::SET_KILLED_BLOODVESSEL( killed_bloodvessel_ );
		T3E::SCORE::SET_KILLED_STEM_CELL( killed_stem_cell_ );
		T3E::SCORE::SET_KILLED_ARRESTED_CELL( killed_arrested_cell_ );

		T3E::SCORE::SET_CANCER_PER_SECOND( cancer_per_second_ );
	}

    // Set the first cell
	T3E::Cell* firstStem;
    grid_.newCell( 21, 23, T3E::CellState::STEM, 0, &firstStem );
	firstStem->ignoreBirthDelay();
    firstStem = nullptr;
	
	// Set a test blood vessel
    grid_.newBloodVessel( 24, 24, nullptr );
	
	//init the hex vertex buffer
	glGenBuffers(1, &hexBufferName);
	std::vector<GLfloat> hexVerts;
	float size = 0.54;//should get from hex?
	float sizeCos30 = size*glm::cos(glm::radians(30.0f));
	float sizeSin30 = size*glm::sin(glm::radians(30.0f));
	
	hexVerts.push_back( 0.0f ); // x1
	hexVerts.push_back( size ); // y1
	hexVerts.push_back( -sizeCos30 ); // x2
	hexVerts.push_back( sizeSin30 ); // y2
	hexVerts.push_back( -sizeCos30 ); // x3
	hexVerts.push_back( -sizeSin30 ); // ...
	hexVerts.push_back( 0.0f );
	hexVerts.push_back( -size );
	hexVerts.push_back( sizeCos30 );
	hexVerts.push_back( -sizeSin30 );
	hexVerts.push_back( sizeCos30 );
	hexVerts.push_back( sizeSin30 );
	hexVerts.push_back( 0.0f ); // back to the start
	hexVerts.push_back( size ); // 

	glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * hexVerts.size(), hexVerts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	

	//Create buttons
	menuButton_.init(float(window_->getScreenWidth())/100.0f, float(window_->getScreenHeight())*(8.9f/10.0f),
		float(window_->getScreenHeight())/10.0f, float(window_->getScreenHeight())/10.0f, "textures/ssheet0.png",
		0.99f/10, 1.0f/10,
		4.99f/10, 8.0f/10,
		4.99f/10, 9.0f/10);
	
 	bvButton_.init(float(window_->getScreenWidth())/100.0f, float(window_->getScreenHeight())*(7.8f/10.0f),
		float(window_->getScreenHeight())/10.0f, float(window_->getScreenHeight())/10.0f, "textures/ssheet0.png",
		0.98f/10, 1.0f/10,
		5.98f/10, 8.0f/10,
		5.98f/10, 9.0f/10);
		
 	killButton_.init(float(window_->getScreenWidth())/100.0f, float(window_->getScreenHeight())*(6.7f/10.0f),
		float(window_->getScreenHeight())/10.0f, float(window_->getScreenHeight())/10.0f, "textures/ssheet0.png",
		0.99f/10, 1.0f/10,
		6.97f/10, 8.0f/10,
		6.97/10, 9.0f/10);
	
	resumeButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(4.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		2.0f/16, 2.0f/4,
		2.0f/16, 3/4.0f);
		
 	quitButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(2.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		5.0f/16, 2.0f/4,
		5.0f/16, 3/4.0f);
        
    scorebar_.init(window_->getScreenWidth()/100.0f + float(window_->getScreenHeight())/10.0f, float(window_->getScreenHeight())*(8.43f/10.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/ssheet0.png",
		1.0f/13, 1.0f/4,
		1.0f/2, 1.0f/2,
		1.0f/2, 1.0f/2);
	
    nextButton_.init( float(window_->getScreenWidth())*0.85f, float(window_->getScreenHeight())*0.05f,
		float(window_->getScreenWidth())*0.12f, float(window_->getScreenHeight())*0.08f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		6.0f/16, 2.0f/4,
		6.0f/16, 3/4.0f );

	// background sprite
	backgroundSprite_.init(0.0f, 0.0f, float(window_->getScreenWidth()), float(window_->getScreenHeight()),"textures/background.png", 0, 0, 1.0f, 1.0f);

	// Set the inital score
	int initial_score_;
	configFile.getInt("initial_score", &initial_score_, 0 );
	grid_.setScore( initial_score_ );

	// init shaders
	initShaders();
}

void MainGame::initShaders()
{
	// CELL SHADER
	tintedSpriteProgram_.compileShaders("shaders/tintedSprite_vs.txt", "shaders/tintedSprite_ps.txt");
	tintedSpriteProgram_.addAttribute("aPosition");
	tintedSpriteProgram_.addAttribute("aTexCoord");
	tintedSpriteProgram_.linkShaders();

	// query uniform locations - could use "layout location" in shaders to set fixed locations
	cell_finalM_location	= tintedSpriteProgram_.getUniformLocation("finalM");
	sampler0_location		= tintedSpriteProgram_.getUniformLocation("sampler0");
	inputColour_location	= tintedSpriteProgram_.getUniformLocation("inputColour");
	
	// HEX SHADER This shader draws the grid
	hexProgram_.compileShaders("shaders/hex_vs.txt", "shaders/hex_ps.txt");
	hexProgram_.addAttribute("aPosition");
	hexProgram_.linkShaders();

	// query uniform locations - could use "layout location" in shaders to set fixed locations
	range_location			= hexProgram_.getUniformLocation("range");
	lerp_weight_location	= hexProgram_.getUniformLocation("weight");
	avaliable_for_highlight	= hexProgram_.getUniformLocation("Avaliable");
	hex_finalM_location		= hexProgram_.getUniformLocation("finalM");

	/* UI Shader: draws buttons and other ui elements (excluding text) */
	uiProgram_.compileShaders("shaders/ui_vs.txt", "shaders/ui_ps.txt");
	uiProgram_.addAttribute("aPosition");
	uiProgram_.addAttribute("aTexCoord");
	uiProgram_.linkShaders();

	// query uniform locations
	ui_finalM_location_	= uiProgram_.getUniformLocation("finalM");
	ui_sampler_location_ = uiProgram_.getUniformLocation("sampler0");
}

command MainGame::gameLoop()
{
	Uint32 old_ticks = 0;
	Uint32 ticks = 0;

	timer_.set( 3000 );

	//our game loop
	while( gameState_ != GameState::EXIT )
	{
		// used for frame time measuring
		float startTicks = SDL_GetTicks();		
		calculateFPS();
			
		if( grid_.getCurrency() > 0 )
		{
			if( !paused_ )
			{
				if( grid_.update( frameTime_, world_to_grid( finger_position_world_ ) ) )
					cellSelected_ = false;
			
				if(grid_.playVessel())
				{
					blood_vessel_sound_.play();
					grid_.resetPlayVessel();
				}
			
				// Count the number of cancer cells
				int num_cancer_cells = 0;
				for( int i = 0; i < grid_.numCells(); ++i ) {
					if( ((T3E::Cell*)grid_.getCell(i)->getNode())->getState() == T3E::CellState::CANCEROUS ) num_cancer_cells++;
				}

				// if a second has passed, reduce the score by the score per cancer per second
				if( old_ticks / 1000 < ticks / 1000 ) {
					grid_.addScore( num_cancer_cells * T3E::SCORE::CANCER_PER_SECOND() );
				}
			}
		}
		else // game over!!!
		{
			gameOver_ = true;
			textRenderer_->putString( "Game over!", -0.5, 0.3, 0.1f );
		}
		
		score_ = grid_.getHighScore();
        textRenderer_->putNumber( grid_.getHighScore() * 100, 10, -0.77, 0.93, 0.06f );
		textRenderer_->putNumber( grid_.getCurrency(), 6, -0.82, 0.785, 0.06f );

		// Render the tutorial text
		if( tutorial_ )	
			renderTutorial();

		renderGame();

		processInput(frameTime_);

		float frameTicks = SDL_GetTicks() - startTicks;
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > frameTicks)
		{
			SDL_Delay(1000.0f / maxFPS_ - frameTicks);
		}

		old_ticks = ticks;
		ticks = SDL_GetTicks();
	}
	
	return command::MENU;
}

void MainGame::processInput( float dTime )
{
	glm::vec2 screenCoords;
	finger_pressed_ = false;
	finger_lifted_ = false;
	
	// processing our input
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		if( event.type == SDL_QUIT )
		{
			gameState_ = GameState::EXIT;
			continue;
		}

		// Generic event handeling
		switch( event.type )
		{
		case SDL_FINGERDOWN:
			nOfFingers_++;
			finger_down_ = true;
			finger_pressed_ = true;

			update_finger_position( event.tfinger.x, event.tfinger.y );

			// Store where the finger was placed on the sceen
			finger_down_position_sdl_ = finger_position_sdl_;
		break;
		case SDL_FINGERUP:
			nOfFingers_--;
			pressTimer_ = 0;
			finger_down_ = false;
			finger_lifted_ = true;

			update_finger_position( event.tfinger.x, event.tfinger.y );

			// When the last finger is lifted, reset the drag
			if( nOfFingers_ == 0 ) finger_dragged_ = false;
		break;
		case SDL_FINGERMOTION:

			update_finger_position( event.tfinger.x, event.tfinger.y );

			// Only register the finger as dragged if it moves over a certain distance from where it was placed
			if(    std::abs(event.tfinger.x - finger_down_position_sdl_.x) > 0.04f
				|| std::abs(event.tfinger.y - finger_down_position_sdl_.y) > 0.04f )
			{
				finger_dragged_ = true;
			}

			// Move the camera with a one finger drag
			if( nOfFingers_ < 2 && finger_dragged_ && !paused_ )
			{
				camera_.moveDelta( glm::vec3( -event.tfinger.dx, event.tfinger.dy, 0.0f) );
				
				// Wait for the player to drag the camera before advancing the tutorial
				if( tut_phase_ == TutorialPhase::MOVE_CAM )
					increment_tutorial();
			}
		break;
		case SDL_MULTIGESTURE:
			if( !paused_ )
			{
				if( tut_phase_ == TutorialPhase::ZOOM_CAM )
					increment_tutorial();

				// pinch gesture to zoom
				camera_.zoom( -event.mgesture.dDist * ZOOM_SENSITIVITY );
			}
		break;
		case SDL_KEYDOWN:
			// Toggle pause with the android back key
			if( event.key.keysym.sym == SDLK_AC_BACK )
			{
				paused_ = !paused_;
				paused_? menuButton_.press() : menuButton_.unpress();
			}	

			if( !paused_ )
			{
				// emulate zoom with z and x keys when running in emulator
				// Zoom in with z
				if( event.key.keysym.sym == SDLK_z )
				{
					camera_.zoom(-0.1f);

					if( tut_phase_ == TutorialPhase::ZOOM_CAM )
						increment_tutorial();
				}

				// Zoom out with x
				if( event.key.keysym.sym == SDLK_x ) camera_.zoom(0.1f);

				if( tut_phase_ == TutorialPhase::ZOOM_CAM )	increment_tutorial();
			}
		break;
		default: break;
		}
	}
	
	if( gameOver_ )
	{
		if( finger_down_ )
		{
			if( quitButton_.touchCollides( finger_position_pixels_ ) ) quitButton_.press();
			else quitButton_.unpress();
		}
		else if( finger_lifted_ && nOfFingers_ == 0 )
		{
			if( quitButton_.touchCollides( finger_position_pixels_ ) )
				gameState_ = GameState::EXIT;

			quitButton_.unpress();
		}
	}
	else if( paused_ )
	{
		if( finger_down_ )
		{
			// Display a button as pressed if the finger is over it
			if( resumeButton_.touchCollides( finger_position_pixels_ ) ) resumeButton_.press();
			else resumeButton_.unpress();

			if( quitButton_.touchCollides( finger_position_pixels_ ) ) quitButton_.press();
			else quitButton_.unpress();
		}
		else if( finger_lifted_ && nOfFingers_ == 0 )
		{
			// Only activate the button if the finger was lifted while over it
			if( resumeButton_.touchCollides( finger_position_pixels_ )
				|| menuButton_.touchCollides( finger_position_pixels_ ) )
			{
				resumeButton_.unpress();
				menuButton_.unpress();
				paused_ = false;

				if( tut_phase_ == TutorialPhase::SHOW_PAUSE )
					increment_tutorial();
			}
			else if( quitButton_.touchCollides( finger_position_pixels_ ) )
			{
				gameState_ = GameState::EXIT;
				quitButton_.unpress();
			}
		}
	}
	else // We are playing normally
	{
		if( finger_down_ )
		{
			// press the buttons if the user is hovering over them, but don't activate them untill they release
			if( interactionMode_ == InteractionMode::NORMAL )
				menuButton_.touchCollides( finger_position_pixels_ )? menuButton_.press() : menuButton_.unpress();
			if( interactionMode_ != InteractionMode::BVCREATION )
				bvButton_.touchCollides( finger_position_pixels_ )? bvButton_.press() : bvButton_.unpress();
			if( interactionMode_ != InteractionMode::KILLMODE )
				killButton_.touchCollides( finger_position_pixels_ )? killButton_.press() : killButton_.unpress();
			if( tutorial_ )
				nextButton_.touchCollides( finger_position_pixels_ )? nextButton_.press() : nextButton_.unpress();
		}
		else
		{
			if( tutorial_ )
				nextButton_.unpress();
		}		

		if( finger_lifted_ )
		{
			if( tutorial_ )
			{
				if( nextButton_.touchCollides( finger_position_pixels_ ) )
				{
					increment_tutorial();
				}
			}

			// Check if the blood vessel button has been pressed
			if( bvButton_.touchCollides( finger_position_pixels_) )
			{
				if( interactionMode_ == InteractionMode::BVCREATION )
				{
					interactionMode_ = InteractionMode::NORMAL;
					bvButton_.unpress();
				}
				else
				{
					interactionMode_ = InteractionMode::BVCREATION;
					bvButton_.press();
					killButton_.unpress();
				}

				// Unselect cell
				grid_.unselectCell( selectedPos_.x, selectedPos_.y );
				cellSelected_ = false;
			}
			// Check if the kill cell button has been pressed
			else if( killButton_.touchCollides( finger_position_pixels_ ) )
			{
				if( interactionMode_ == InteractionMode::KILLMODE )
				{
					interactionMode_ = InteractionMode::NORMAL;
					killButton_.unpress();
				}
				else
				{
					interactionMode_ = InteractionMode::KILLMODE;
					killButton_.press();
					bvButton_.unpress();
				}
				
				// Unselect cell
				grid_.unselectCell( selectedPos_.x, selectedPos_.y );
				cellSelected_ = false;	
			}
			// Check if the pause menu button has been pressed
			else if( menuButton_.touchCollides( finger_position_pixels_ ) )
			{
				paused_ = !paused_;
				paused_ ? menuButton_.press() : menuButton_.unpress();
			}
			else
			{
				switch( interactionMode_ )
				{
				case InteractionMode::NORMAL:
					if( cellSelected_ )
					{								
						//try to spawn
						if(!grid_.spawnCell(selectedPos_.x, selectedPos_.y, finger_position_row_col_.x, finger_position_row_col_.y))
						{

							if( grid_.moveStemCell(selectedPos_.x, selectedPos_.y, finger_position_row_col_.x, finger_position_row_col_.y) )
							{
								// If the stem cell moved successfully
								if( tut_phase_ == TutorialPhase::MOVE_STEM ) {
									increment_tutorial();
								}
							}																			
						}	
						else
						{
							select_sound_.play();
						}
						
						if( finger_position_row_col_.x == selectedPos_.x && finger_position_row_col_.y == selectedPos_.y )
						{
							grid_.unselectCell(selectedPos_.x, selectedPos_.y);//move inside select cell?
							cellSelected_ = false;
						}
						else
						{
							grid_.unselectCell(selectedPos_.x, selectedPos_.y);//move inside select cell?
							cellSelected_ = false;
							//try to select a cell
							//also, if a new cell was created, select it
							selectCell( finger_position_row_col_.x, finger_position_row_col_.y );
							//cellSelected_ = true;
						}
						break;
					}
					// try to select a cell
					// also, if a new cell was created, select it		
					selectCell( finger_position_row_col_.x, finger_position_row_col_.y );

				break;
				case InteractionMode::KILLMODE:
					if( grid_.killCell(finger_position_row_col_.x, finger_position_row_col_.y) )
					{
						// play kill noise
						cell_death_sound_.play();
						
						if( tut_phase_ == TutorialPhase::KILL_CELL )
							increment_tutorial();									
					}
				break;
				case InteractionMode::BVCREATION: break;
				default: break;
				}	
			}
		}
	}

	//check for finger pressure
	if( finger_down_ && !finger_dragged_  && !paused_)
	{
		pressTimer_ += dTime;
		if(pressTimer_ >= 800)
		{
			pressTimer_ = 0;
			finger_down_ = false;
			
			if( interactionMode_ == InteractionMode::NORMAL )
			{
				//try to arrest
				if(!grid_.arrestCell(finger_position_row_col_.x, finger_position_row_col_.y, &cellSelected_))
				{
					//try to change stem cell mode
					if(grid_.setStemToSpawnMode(finger_position_row_col_.x, finger_position_row_col_.y))
						cell_mode_change_sound_.play();
				}					
				else
				{
					cell_arrest_sound_.play();
					if( tut_phase_ == TutorialPhase::ARREST_CELL )
						increment_tutorial();
				}
			}
			else if( interactionMode_ == InteractionMode::BVCREATION )
			{
				//try to set bv spawn point
				if( grid_.setBvSpawn(finger_position_row_col_.x, finger_position_row_col_.y) )
				{
					bvButton_.unpress();
					interactionMode_ = InteractionMode::NORMAL;

					if( tut_phase_ == TutorialPhase::PLACE_BV )
						increment_tutorial();
				}
			}
		}
	}	
}

void MainGame::renderGame()
{
	// clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	static const float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
	static const float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// update matrices
	viewM_ = glm::lookAt(camera_.getPosition(), camera_.getLookAt(), camera_.getUp());
	finalM_ = projectionM_ * viewM_ * worldM_; // order matters!
	// We can precalculate this because it's the same for each call or render game
	glm::mat4 projectionView = projectionM_ * viewM_;
	
	/* render background */
	tintedSpriteProgram_.use();
	// send ortho matrix to shaders
	glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	glUniform4fv( inputColour_location, 1, white);
	// set texture	
	glActiveTexture( GL_TEXTURE0 + backgroundSprite_.getTexUnit() );	
	glUniform1i( sampler0_location, backgroundSprite_.getTexUnit() );
	// draw the background image, then the hex grid on top
	backgroundSprite_.draw();
	drawGrid();		
	
	tintedSpriteProgram_.use();

	// Render blood vessels
	{
		GLuint bvUnit = T3E::ResourceManager::getTexture("textures/bloodVessel.png").unit;
		glActiveTexture( GL_TEXTURE0 + bvUnit );
		glUniform1i( sampler0_location, bvUnit );
	
		for(int i = 0; i < grid_.numBloodVessels(); ++i)
		{
			T3E::BloodVessel* current = (T3E::BloodVessel*)grid_.getBloodVessel(i)->getNode();
			
			//move to hex position
			glm::mat4 worldM_;
			worldM_ = glm::translate( worldM_, glm::vec3( grid_.getBloodVessel(i)->getX(), grid_.getBloodVessel(i)->getY(), 0.0f ) );
			glm::mat4 finalM_ = projectionView * worldM_;
			
			//send matrix to shaders
			glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
			
	        //use texture 1
			current->getSprite()->draw();
		}
	}	
	// Render position checkers of bv spawn points
	{
		glActiveTexture( GL_TEXTURE0 + sprites_[0]->getTexUnit() );
		glUniform1i( sampler0_location, sprites_[0]->getTexUnit() );

		float tint[] = { 0.0f, 0.0f, 1.0f, 0.3f };
		glUniform4fv(inputColour_location, 1, tint);

		for(int i = 0; i < grid_.numBvSpawns(); ++i)
		{
			glm::vec2 coords = grid_.getBvSpawnCoords(i);
			
			// move to hex position
			glm::mat4 worldM_;
			worldM_ = glm::translate( worldM_, glm::vec3( coords.x, coords.y, 0.0f ) );
			finalM_ = projectionView * worldM_;
			
			// send matrix to shaders
			glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_) );
			
			sprites_[0]->draw();
		}
	}
	{
		// Render cells
		//GLuint cellTexture = T3E::ResourceManager::getTexture("textures/cellSheet.png").unit;
		//glActiveTexture( GL_TEXTURE0 + cellTexture );
		//glUniform1i( sampler0_location, cellTexture );
		for(int i = 0; i < grid_.numCells(); ++i)
		{
			T3E::Cell* current = (T3E::Cell*)grid_.getCell(i)->getNode();
			
			// move to hex position
			glm::mat4 worldM_;
			worldM_ = glm::translate( worldM_, glm::vec3( grid_.getCell(i)->getX(), grid_.getCell(i)->getY(), 0.0f ) );

			// Rotate splitting cells to face the direction they are splitting
			if( current->isSplitting() )
				worldM_ = glm::rotate(worldM_, glm::radians(current->getSplitRotation()), glm::vec3(0.0f, 0.0f, 1.0f));

			glm::mat4 finalM_ = projectionView * worldM_;
			
			// send matrix to shaders
			glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_) );

			// set tint
			float tint[] = {current->getTint().x ,current->getTint().y , current->getTint().z, current->getTint().w};
			glUniform4fv(inputColour_location, 1, tint);

			current->draw();
		}
	}
	
	/* Render User Interface */
	uiProgram_.use();

	// send ortho matrix to shaders
	glUniformMatrix4fv( ui_finalM_location_, 1, GL_FALSE, glm::value_ptr(orthoM_) );

	// All the button textures are in one texture so we only have to do this once
	glActiveTexture( GL_TEXTURE0 + menuButton_.getTexUnit() );
	glUniform1i( ui_sampler_location_, menuButton_.getTexUnit() );

	menuButton_.draw();
	bvButton_.draw();
	killButton_.draw();	
    scorebar_.draw();

	// Render the tex before the menus so the menus appear on top
	textRenderer_->render();
    
    // Now render the rest of th UI
	uiProgram_.use();
    
    if( tutorial_ )
    {
    	nextButton_.draw();
    }
    
	// Render menus
	if( paused_ )
	{
		resumeButton_.draw();
		quitButton_.draw();
	}		
	else if( gameOver_ )
	{
		quitButton_.draw();
		grid_.setCurrency( 0 );
	}

	// swap our buffers, display the frame
	window_->swapBuffer();
}

void MainGame::update_finger_position( float x, float y )
{
	// Position of the finger on screen in normalized coords, from (0,1)
	finger_position_sdl_ = glm::vec2( x, y );

	// Convert the positon of the finger on the touch screen to world coords
	finger_position_world_ = touch_to_world( finger_position_sdl_ );

	// Get the position of the touch in screen pixels
	finger_position_pixels_ = touch_to_pixels( finger_position_sdl_ );

	// Find the closest row and column of the touch position
	finger_position_row_col_ = world_to_grid( finger_position_world_ );
}

glm::vec2 MainGame::touch_to_world( glm::vec2 touch_coord )
{
    glm::vec4 result( touch_coord.x, touch_coord.y, 0.0f, 1.0f );

    result.x = result.x * 2.0f - 1.0f;
    result.y = result.y * 2.0f - 1.0f;
    result.y *= -1.0f; // Invert to match OpenGL coords
    
    // Calculate the inverse matrix of the view and projection
    viewProjInverse = glm::inverse( projectionM_ * viewM_ );

    // multiply the touch position by that
    result = viewProjInverse * result;
    
    // divide by w
    result.x /= result.w;
    result.y /= result.w;

    // scale the position to account for zoom
    result.x = camera_.getPosition().x + ( result.x - camera_.getPosition().x ) * result.w * camera_.getPosition().z;
    result.y = camera_.getPosition().y + ( result.y - camera_.getPosition().y ) * result.w * camera_.getPosition().z;

    return glm::vec2( result.x, result.y );
}

SDL_Point MainGame::touch_to_pixels( glm::vec2 touch_coord )
{
	SDL_Point result;
	result.x = std::floor( touch_coord.x * window_->getScreenWidth() );
	result.y = std::floor( window_->getScreenHeight() - touch_coord.y * window_->getScreenHeight() );
	return result;
}

SDL_Point MainGame::world_to_grid( glm::vec2 world_coord )
{
	float fracCol, fracRow, fracZ;//fractional coordinates
	int col, row, z;//final coordinates
	float colD, rowD, zD;//difference between fractional and rounded coords
	
    glm::mat2 layoutInverse_= glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
	fracCol = (layoutInverse_[0][0] * world_coord.x + layoutInverse_[0][1] * world_coord.y) / 0.54f;
	fracRow = (layoutInverse_[1][0] * world_coord.x + layoutInverse_[1][1] * world_coord.y) / 0.54f;

	fracZ = -fracCol -fracRow;
    
    col = round(fracCol);
    row = round(fracRow);
    z = round(fracZ);
    
	colD = abs(col - fracCol);
    rowD = abs(row - fracRow);
    zD = abs(z - fracZ);
    
    if((colD > rowD) && (colD > zD))
    {
        col = -row-z;
    }
    else if(rowD > zD)
    {
        row = -col-z;
    }
    else
	{
        z = -row-col;
	}

    return SDL_Point{ row, col};
}

bool MainGame::selectCell(int row, int col)
{
	//if the coordinates contain a cell
	if( grid_.selectCell(row, col) )
	{
		cellSelected_ = true;
		selectedPos_ = glm::vec2(row, col);
		return true;
	}
	cellSelected_ = false;
	return false;
}

void MainGame::calculateFPS()
{
	static const int NUM_SAMPLES = 1000;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;
	static float prevTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	frameTime_ = currentTicks - prevTicks;
	
	//avoid bug that set frameTime_ to very low negative number on first calculation
	if(frameTime_ < 0)
		frameTime_ = 0;
	
	frameTimes[currentFrame % NUM_SAMPLES] = frameTime_;

	// set previous ticks to new ticks now
	prevTicks = currentTicks;

	currentFrame++;

	int count;

	if (currentFrame < NUM_SAMPLES)
	{
		count = currentFrame;
	}
	else
	{
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++)
	{
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if (frameTimeAverage > 0)
	{
		fps_ = 1000.0 / frameTimeAverage;
	}
	else
	{
		fps_ = 60.0f;
	}
}

void MainGame::renderTutorial()
{
	// Depending on the tutorial phase,
	// sometimes the tutorial will progress with just a tap,
	// sometimes the player has to perform a certain action
	if( finger_pressed_ && (
		tut_phase_ == TutorialPhase::READY ||
		tut_phase_ == TutorialPhase::SHOW_PAUSE ||
		tut_phase_ == TutorialPhase::SHOW_SCORE ||
		tut_phase_ == TutorialPhase::SHOW_CURRENCY ||
		tut_phase_ == TutorialPhase::EXPLAIN_STEMBV ||
		tut_phase_ == TutorialPhase::MUTATE_CELL ||
		tut_phase_ == TutorialPhase::CANCER_CELL ||
		tut_phase_ == TutorialPhase::DONE
		) )
	{
		//increment_tutorial();
	}

	switch( tut_phase_ )
	{
	case TutorialPhase::READY:
		textRenderer_->putString( "Welcome to the\nCell Cycle tutorial.\n\nTap next to begin", -0.8, 0.6, 0.07f );
	break;
	case TutorialPhase::MOVE_CAM:
		textRenderer_->putString( "Swipe the screen to\nscoll the camera.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::ZOOM_CAM:
		textRenderer_->putString( "Use a pinch gesture\nto zoom in and out.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::SHOW_PAUSE:
		textRenderer_->putString( "The button in the top left\npauses the game.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::SHOW_SCORE:
		textRenderer_->putString( "The top number is your score\n\nYour score will increase as you\ngrow healthy cells.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::SHOW_CURRENCY:
		textRenderer_->putString( "The bottom number is your\ncurrenncy, keep an eye on it!\n\nSome actions earn you points\nand currency,while others\nwill cost you.\n\nIf your currency hits 0 it is\ngame over!", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::EXPLAIN_STEMBV:
		textRenderer_->putString( "Here you see a blood vessel\nand a stem cell.\n\nStem cells produce healthy cells\nwhile blood vessels provide\nenergy to the cells\naround them.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::MOVE_STEM:
		textRenderer_->putString( "Tap a stem cell,\nthen tap an adjacent hex\nto move it", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::SPLIT_STEM:
		textRenderer_->putString( "Hold your finger on a stem\ncell to put it into split mode.\n\nThen tap an adjacent hex\nto create a new stem cell", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::PLACE_BV:
		textRenderer_->putString( "Tap the blood vessel button,\nthen place a blood vessel spawn\nby holding on a hex.\n\nIf you put one in the wrong place\npress the blood vessel button\nagain and hold to remvoe it", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::CREATE_BV:
		textRenderer_->putString( "Move the stem cell to the centre\nof the blood vessel spawn.\n\nThen surround it with healthy\ncells to create a new\nblood vesesl.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::MUTATE_CELL:
		textRenderer_->putString( "Sometimes healthy cells will\nspawn orange mutated cells!\n\nMutated cell aren't dangerous\nthemselves but they can lead\nto dangerous mutations", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::ARREST_CELL:
		textRenderer_->putString( "You can control the board by\ncreating barriers of\narrested cells.\n\nTo arrest a cell hold down\non a healthy cell till\nit changes colour.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::KILL_CELL:
		textRenderer_->putString( "A more effective but costly\nway to control mutated cells is\nto use the kill button,\nGive it a try.\n\n(The lowest button on the left)", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::CANCER_CELL:
		textRenderer_->putString( "Purple cells are dangerously\nmutated!\n\nYou cant kill these or arrest\nthem, but you can isolate\nthem with arrested cells.", -0.8, 0.6, 0.06f );
	break;
	case TutorialPhase::DONE:
		textRenderer_->putString( "And thats all there is to learn.\n\nWe hope you enjoy Cell Cycle!", -0.8, 0.4, 0.06f );
	break;
	default:
	break;
	}

	// Turn on the grid once we start checking for blood vessels
	if( tut_phase_ == TutorialPhase::PLACE_BV )
	{
		grid_.setGridUpdates( true );
	}
}

void MainGame::increment_tutorial()
{
	if( tut_phase_ == TutorialPhase::READY ) tut_phase_ = TutorialPhase::MOVE_CAM;
	else if( tut_phase_ == TutorialPhase::MOVE_CAM ) tut_phase_ = TutorialPhase::ZOOM_CAM;
	else if( tut_phase_ == TutorialPhase::ZOOM_CAM ) tut_phase_ = TutorialPhase::SHOW_PAUSE;
	else if( tut_phase_ == TutorialPhase::SHOW_PAUSE ) tut_phase_ = TutorialPhase::SHOW_SCORE;
	else if( tut_phase_ == TutorialPhase::SHOW_SCORE ) tut_phase_ = TutorialPhase::SHOW_CURRENCY;
	else if( tut_phase_ == TutorialPhase::SHOW_CURRENCY ) tut_phase_ = TutorialPhase::EXPLAIN_STEMBV;
	else if( tut_phase_ == TutorialPhase::EXPLAIN_STEMBV ) tut_phase_ = TutorialPhase::MOVE_STEM;
	else if( tut_phase_ == TutorialPhase::MOVE_STEM ) tut_phase_ = TutorialPhase::SPLIT_STEM;
	else if( tut_phase_ == TutorialPhase::SPLIT_STEM ) { tut_phase_ = TutorialPhase::PLACE_BV; }
	else if( tut_phase_ == TutorialPhase::PLACE_BV ) tut_phase_ = TutorialPhase::CREATE_BV;
	else if( tut_phase_ == TutorialPhase::CREATE_BV ) tut_phase_ = TutorialPhase::MUTATE_CELL;
	else if( tut_phase_ == TutorialPhase::MUTATE_CELL ) tut_phase_ = TutorialPhase::ARREST_CELL;
	else if( tut_phase_ == TutorialPhase::ARREST_CELL ) tut_phase_ = TutorialPhase::KILL_CELL;
	else if( tut_phase_ == TutorialPhase::KILL_CELL ) tut_phase_ = TutorialPhase::CANCER_CELL;
	else if( tut_phase_ == TutorialPhase::CANCER_CELL ) tut_phase_ = TutorialPhase::DONE;
	else if( tut_phase_ == TutorialPhase::DONE )
	{
		tut_phase_ = TutorialPhase::NONE;
		tutorial_ = false;
	}

}

void MainGame::drawGrid()
{
	//draw highlighted hexes last so they're always on top of the others
	std::vector<glm::vec4> hexesInRange;
	std::vector<glm::vec4> hexesInLargeRange;
	std::vector<glm::vec4> yellowHexes;

	hexProgram_.use();

	// TODO: replace this shitty excure for an enum
	// The getHexDrawInfo function should really be redsigned
	static const float CLOSE_RANGE_OF_BV = 0.0f;
	static const float FAR_RANGE_OF_BV = 1.0f;
	static const float YELLOW = 3.0f;

	// Send the verts to the gpu and specify the layout
	glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
					
	//send range info
	glUniform1f(range_location, 2.0f );
	//set whether or not to highlight the grid
	glUniform1i(avaliable_for_highlight, 0);

	for(int r = 0; r < grid_.getSize(); ++r)
	{
		for(int c = 0; c < grid_.getSize(); ++c)
		{
			glm::vec4 drawData = grid_.getHexDrawInfo(r, c, cellSelected_, selectedPos_, interactionMode_);
			//if hex exists
			if(drawData.x != -1)
			{
				//if in range of blood vessel draw it later
				if(drawData.z == CLOSE_RANGE_OF_BV )
				{				
					hexesInRange.push_back(drawData);
				}
				else if(drawData.z == FAR_RANGE_OF_BV )
				{
					hexesInLargeRange.push_back(drawData);
				}
				else if(drawData.z == YELLOW )
				{
					yellowHexes.push_back(drawData);					
				}
				else // hex is not in any range of any bv, draw it normaly
				{
					//send matrix to shaders
					glm::mat4 tranlation_matrix = glm::translate(worldM_, glm::vec3(drawData.x, drawData.y, 0.0f));
					glm::mat4 final_matrix = projectionM_ * viewM_ * tranlation_matrix;			
					glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(final_matrix));
					
					glDrawArrays(GL_LINE_STRIP, 0, 4);
					
					// Fill in the gaps at the edges
					if( r == 0 && c < grid_.getSize() - 1 )
						glDrawArrays( GL_LINES, 3, 2 );
					if( r == grid_.getSize() - 1 )
						glDrawArrays( GL_LINES, 5, 2);
					if( c == grid_.getSize() - 1 )
						glDrawArrays( GL_LINE_STRIP, 3, 3 );
				}
			}
		}
	}	
	
	//send range info
	glUniform1f(range_location, CLOSE_RANGE_OF_BV);

	//now draw hexes in range
	for(std::vector<glm::vec4>::iterator data = hexesInRange.begin(); data != hexesInRange.end(); ++data)
	{
		//send matrix to shaders
		glm::mat4 tranlation_matrix = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		glm::mat4 final_matrix = finalM_ * tranlation_matrix;			
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(final_matrix));

		//send distance info
		glUniform1f(lerp_weight_location, data->w);

		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0, 6);
	}

	//send range info
	glUniform1f(range_location, FAR_RANGE_OF_BV);

	//now draw hexes in large range
	for(std::vector<glm::vec4>::iterator data = hexesInLargeRange.begin(); data != hexesInLargeRange.end(); ++data)
	{
		//send matrix to shaders
		glm::mat4 tranlation_matrix = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		glm::mat4 final_matrix = finalM_ * tranlation_matrix;			
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(final_matrix));

		//send distance info
		glUniform1f(lerp_weight_location, data->w);

		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0,6);
	}
	
	//send range info
	glUniform1f(range_location, YELLOW);
	//set whether or not to highlight the grid
	glUniform1i(avaliable_for_highlight, 1);

	//now draw yellow hexes
	for(std::vector<glm::vec4>::iterator data = yellowHexes.begin(); data != yellowHexes.end(); ++data)
	{
		//send matrix to shaders
		//translate world matrix to separate triangles and create parallax
		glm::mat4 tranlation_matrix = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		glm::mat4 final_matrix = finalM_ * tranlation_matrix;			
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(final_matrix));

		//send distance info
		glUniform1f(lerp_weight_location, data->w);

		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0, 6);
	}
}