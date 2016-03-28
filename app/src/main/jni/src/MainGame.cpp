#include "MainGame.h"
#include "Type3Engine/ConfigFile.h"
#include "GlobalScoreValues.h"

MainGame::MainGame() : 
	screenHeight_(800),
	screenWidth_(600),
	time_(0.0f), 
	score_(0),
	gameState_(GameState::PLAY),
	maxFPS_(60.0f),
	nOfFingers_(0),
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(6.0f),
    finger_dragged_(false),
	pressTimer_(0),
	fingerPressed_(false),
	cellSelected_(false),
	interactionMode_(InteractionMode::NORMAL)
{}

MainGame::~MainGame()
{
	//free memory
	for (std::vector<T3E::Sprite*>::iterator it = sprites_.begin() ; it != sprites_.end(); ++it) {
		delete (*it);
	} 
	sprites_.clear();
}

void MainGame::run()
{
	initSystems();
	
	//bloodVessel TODO: put in bv class like cell
 	sprites_.push_back( new T3E::Sprite() );
	sprites_.back()->init(-1.5f, -1.5f, 3.0f, 3.0f,"textures/bvSpawnPoint.png", 0.0f, 0.0f, 1.0f, 1.0f);
 
	T3E::Music music = audioEngine_.loadMusic("sound/backgroundSlow.ogg");
	music.play(-1);
	
	bloodV_ = audioEngine_.loadSoundEffect("sound/Blood_Vessel_placeholder.ogg");
	cellMove_ = audioEngine_.loadSoundEffect("sound/Player_CellDivide_Move.ogg");
	
	gameLoop();
}

void MainGame::initSystems()
{
	T3E::init();
	
	audioEngine_.init();
	
	//TODO: change name of window to game name!
	window_.create("Game Engine", screenWidth_, screenHeight_, T3E::BORDERLESS);

	// enable aplha blending	
	glEnable( GL_BLEND );//should we instead use frame buffer fetch in shader?
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	// init projection matrix
	window_.updateSizeInfo(); // can do just once here since screen orientation is set to landscape always
	float ratio = float( window_.getScreenWidth() )/float( window_.getScreenHeight() );	// calculate aspect ratio
	projectionM_ = glm::perspective( 90.0f, ratio, 0.1f, 100.0f ); // fov 90°, aspect ratio, near and far clipping plane
	// init ortho matrix
	// inverting top with bottom to avoid sprites being drawn upside down
	// note that this will put origin at bottom left, while screen coords have origin at top left
	orthoM_ = glm::ortho(0.0f, float( window_.getScreenWidth() ), 0.0f, float( window_.getScreenHeight() ));
	
	// Grab parameters from the config file
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
	}

    // Set the first cell
    grid_.newCell( 21, 23, T3E::CellState::STEM, 0, nullptr );

    // Set a test blood vessel
    grid_.newBloodVessel( 24, 24, nullptr );
	
	//init the hex vertex buffer
	glGenBuffers(1, &hexBufferName);
	
	float size = 0.54;//should get from hex?
	float sizeCos30 = size*glm::cos(glm::radians(30.0f));
	float sizeSin30 = size*glm::sin(glm::radians(30.0f));
	//this is a buffer of lines, so think them 2 by 2
	//1
	hexVertexes[0].setPosition(0.0f, size);
	//hexVertexes[1].setPosition(-sizeCos30, sizeSin30);
	//2
	hexVertexes[1].setPosition(-sizeCos30, sizeSin30);
	//hexVertexes[3].setPosition(-sizeCos30, -sizeSin30);
	//3
	hexVertexes[2].setPosition(-sizeCos30, -sizeSin30);
	//hexVertexes[5].setPosition(0.0f, -size);
	//4
	hexVertexes[3].setPosition(0.0f, -size);
	//hexVertexes[7].setPosition(sizeCos30, -sizeSin30);
	//5
	hexVertexes[4].setPosition(sizeCos30, -sizeSin30);
	//hexVertexes[9].setPosition(sizeCos30, sizeSin30);
	//6
	hexVertexes[5].setPosition(sizeCos30, sizeSin30);
	//hexVertexes[11].setPosition(0.0f, size);
	
	glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hexVertexes), hexVertexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Create ui buttons
 	bvButton_.init(float(window_.getScreenWidth())/30.0f, float(window_.getScreenHeight())*(8.0f/10.0f),
		float(window_.getScreenWidth())/10.0f, float(window_.getScreenWidth())/10.0f, "textures/ui.png",
		0.5, 0.5,
		0, 0,
		0, 0.5);
		
 	killButton_.init(float(window_.getScreenWidth())/30.0f, float(window_.getScreenHeight())*(6.0f/10.0f),
		float(window_.getScreenWidth())/10.0f, float(window_.getScreenWidth())/10.0f, "textures/ui.png",
		0.5, 0.5,
		0.5, 0,
		0.5, 0.5);
	
	//background sprite
	backgroundSprite_.init(0.0f, 0.0f, float(window_.getScreenWidth()), float(window_.getScreenHeight()),"textures/background.png", 0, 0, 1.0f, 1.0f);
	
	textRenderer_.init();
	textRenderer_.setScreenSize( window_.getScreenWidth(), window_.getScreenHeight() );

	// init shaders
	initShaders();
}

void MainGame::initShaders()
{
	 //CELL PRORGAM
	// compile
	tintedSpriteProgram_.compileShaders("shaders/tintedSprite_vs.txt", "shaders/tintedSprite_ps.txt");
	// add attributes
	tintedSpriteProgram_.addAttribute("aPosition");
	tintedSpriteProgram_.addAttribute("aColour");
	tintedSpriteProgram_.addAttribute("aTexCoord");
	// link
	tintedSpriteProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	cell_finalM_location = tintedSpriteProgram_.getUniformLocation("finalM");
	sampler0_location = tintedSpriteProgram_.getUniformLocation("sampler0");
	inputColour_location = tintedSpriteProgram_.getUniformLocation("inputColour");
	
	//HEX PROGRAM
	// compile
	hexProgram_.compileShaders("shaders/hex_vs.txt", "shaders/hex_ps.txt");
	// add attributes
	hexProgram_.addAttribute("aPosition");
	
	// link
	hexProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	range_location = hexProgram_.getUniformLocation("range");
	lerp_weight_location = hexProgram_.getUniformLocation("weight");
	avaliable_for_highlight = hexProgram_.getUniformLocation("Avaliable");
	hex_finalM_location = hexProgram_.getUniformLocation("finalM");
}

void MainGame::gameLoop()
{
	//enable back face culling
	glEnable(GL_CULL_FACE); // GL_BACK is default value
	
	//set line width for grid
	glLineWidth(5.0f);

	int old_score; // The score last frame

	//our game loop
	while( gameState_ != GameState::EXIT )
	{
		// used for frame time measuring
		float startTicks = SDL_GetTicks();		
		time_ += 0.1f;
		calculateFPS();

		if(grid_.update(frameTime_, world_to_grid(touch_to_world(pressPos_))))
			cellSelected_ = false;
		
		if(grid_.playVessel())
		{
			bloodV_.play();
			grid_.resetPlayVessel();
		}
		
		score_ = grid_.getScore();
		textRenderer_.putNumber( score_, 8, -0.05, 0.95, 44 );
		textRenderer_.putString( "T3E Alpha", -1, -0.9, 30 );

		renderGame();
		
		processInput(frameTime_);
		
		// print once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if( frameCounter == 10 )
		{
			//SDL_Log("%f\n", _fps);
			frameCounter = 0;
		}

		float frameTicks = SDL_GetTicks() - startTicks;
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > frameTicks)
		{
			SDL_Delay(1000.0f / maxFPS_ - frameTicks);
		}
	}
}

void MainGame::processInput(float dTime)
{
	glm::vec4 worldPos, worldPos2;
    SDL_Point rowCol, rowCol2;
    int row, col;
    T3E::Hex* neighbours;
	
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch( evnt.type )
		{
		case SDL_QUIT:
			gameState_ = GameState::EXIT;
			break;
			
		case SDL_KEYDOWN:
			if(evnt.key.keysym.sym == SDLK_AC_BACK) // android back key
				gameState_ = GameState::EXIT;
			
			// EMULATOR ZOOM
			if(evnt.key.keysym.sym == SDLK_z)//zoom in
			{
				camera_.zoom(-0.05f);
			}
			if(evnt.key.keysym.sym == SDLK_x)//zoom out
			{
				camera_.zoom(0.05f);
			}
			
			break;
			
		case SDL_FINGERDOWN:
			++nOfFingers_;
			
			fingerPressed_ = true;
			//record position in case we need it later
			pressPos_ = glm::vec2(evnt.tfinger.x, evnt.tfinger.y);
			
            // Convert the touch position to a world position
            worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                        
            // Draw cursor for debug purposes
            cursor_pos_ = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
			break;
			
		case SDL_FINGERUP:
			--nOfFingers_;
			
			fingerPressed_ = false;			
			pressTimer_ = 0;
			pressPos_ = glm::vec2(-1, -1);
            
			// Only act when the last finger is lifted,
            // AND the cursor was not moved
            if( nOfFingers_ == 0 && finger_dragged_ == false )
            {
                // convert the touch to a world position
                worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                // convert the world pos to a grid row column
                rowCol = world_to_grid( worldPos );

				//get touch pos in screen coordinates for UI interaction
				//invert y to match our ortho projection (origin at bottom left for ease of life)
				glm::vec2 screenCoords = glm::vec2(evnt.tfinger.x * float(window_.getScreenWidth()), float(window_.getScreenHeight()) - evnt.tfinger.y * float(window_.getScreenHeight()));					
				if(bvButton_.touchCollides(screenCoords))
				{
					//toggle blood vessel creation mode
					if(interactionMode_ == InteractionMode::BVCREATION)
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
					
					//unselect cell
					grid_.unselectCell(selectedPos_.x, selectedPos_.y);
					cellSelected_ = false;	
				}
				
				if(killButton_.touchCollides(screenCoords))
				{
					//toggle blood vessel creation mode
					if(interactionMode_ == InteractionMode::KILLMODE)
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
					
					//unselect cell
					grid_.unselectCell(selectedPos_.x, selectedPos_.y);
					cellSelected_ = false;	
				}

				switch(interactionMode_)
				{
					case InteractionMode::NORMAL:
					{
						//if a cell was selected
						if(cellSelected_)
						{
							
							//try to spawn
							if(!grid_.spawnCell(selectedPos_.x, selectedPos_.y, rowCol.x, rowCol.y))
							{
								//try to move stem cell
								grid_.moveStemCell(selectedPos_.x, selectedPos_.y, rowCol.x, rowCol.y);
								cellMove_.play();
							}	
							else
							{
								cellMove_.play();
							}
							
							grid_.unselectCell(selectedPos_.x, selectedPos_.y);//move inside select cell?
							//cellSelected_ = false;
							
												
						}
						
						//try to select a cell
						//also, if a new cell was created, select it
						selectCell(rowCol.x, rowCol.y);
						break;
					}
					case InteractionMode::KILLMODE:
					{
						if(!grid_.killCell(rowCol.x, rowCol.y))
						{
							// play error noise
						}
						break;
					}
					case InteractionMode::BVCREATION:
					{
						break;
					}
				}
            }
							
            // Reset the type of touch if the last finger was released
            if( nOfFingers_ == 0 ) finger_dragged_ = false;
			break;
			
		case SDL_FINGERMOTION:
			//avoid microdrag detection
			
			/* Testing another method of microdrag detection
			// convert the touch to a world position
                worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                // convert the world pos to a grid row column
                rowCol = world_to_grid( worldPos );
				
				// convert the touch to a world position
                worldPos2 = touch_to_world( glm::vec2( evnt.tfinger.x + evnt.tfinger.dx, evnt.tfinger.y + evnt.tfinger.dy) );
                // convert the world pos to a grid row column
                rowCol2 = world_to_grid( worldPos );
			
			//SDL_Log("row1x %i, row1y %i, row2x %i, row2y %i", rowCol.x, rowCol.y ,rowCol2.x ,rowCol2.y );
			//if(rowCol.x != rowCol2.x && rowCol.y != rowCol2.y)
			*/
			if(std::abs(evnt.tfinger.dx) > 0.0175 || std::abs(evnt.tfinger.dy) > 0.0175) // when people press down on the screen, they drag way more than just this, older players are less precise == frustration
			{
				finger_dragged_ = true;
				fingerPressed_ = false;	
			}
			
			// pan if only one finger is on screen; you don't want to pan during pinch motion
			if( nOfFingers_ < 2)
			{
				camera_.moveDelta( glm::vec3(-evnt.tfinger.dx, evnt.tfinger.dy, 0.0f) );
			}
			//SDL_Log("%f               %f", evnt.tfinger.dx,evnt.tfinger.dy);
			
			break;
		
		case SDL_MULTIGESTURE: 		
			// pinch zoom
			camera_.zoom( -evnt.mgesture.dDist );
			break;
			
		default: break;
		}
	}
	
	//check for finger pressure
	if(fingerPressed_)
	{
		pressTimer_ += dTime;
		//SDL_Log("%f", pressTimer_);
		if(pressTimer_ >= 800)
		{
			pressTimer_ = 0;
			fingerPressed_ = false;
			rowCol = world_to_grid(touch_to_world(pressPos_));
		
			switch(interactionMode_)
			{
				case InteractionMode::NORMAL:
				{
					//try to arrest
					if(!grid_.arrestCell(rowCol.x, rowCol.y))
						//try to change stem cell mode
						grid_.setStemToSpawnMode(rowCol.x, rowCol.y);	
						
					break;
				}
				case InteractionMode::BVCREATION:
				{
					//try to set bv spawn point
					if(grid_.setBvSpawn(rowCol.x, rowCol.y))
						interactionMode_ = InteractionMode::NORMAL;
				}
				
			}
		}
	}	
}

void MainGame::renderGame()
{
	//clear both buffers
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//update matrices
	viewM_ = glm::lookAt(camera_.getPosition(), camera_.getLookAt(), camera_.getUp());
	finalM_ = projectionM_*viewM_*worldM_;//order matters!
	
	//render background
	//TODO: ideally we want to use and stop use just once per shader, but we need to draw backgruong -> grid -> game elements in this order...
	tintedSpriteProgram_.use();
	// send ortho matrix to shaders
	glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float bgtint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv(inputColour_location, 1, bgtint);
	// set texture	
	glActiveTexture(GL_TEXTURE0+3);	
	glUniform1i(sampler0_location, 3);
	//draw sprite
	backgroundSprite_.draw();
	tintedSpriteProgram_.stopUse();
	
	//RENDER THE HEX GRID
	drawGrid();		
	
	//RENDER CELLS AND BLOOD VESSELS
	tintedSpriteProgram_.use();
	//blood vessels
	for(int i = 0; i < grid_.numBloodVessels(); ++i)
	{
		T3E::BloodVessel* current = (T3E::BloodVessel*)grid_.getBloodVessel(i)->getNode();
		
		//move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( grid_.getBloodVessel(i)->getX(), grid_.getBloodVessel(i)->getY(), 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		//TODO: we don't need to tint blood vessels; remove this and make new shader that doesn't use tint? 
		float tint[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glUniform4fv(inputColour_location, 1, tint);
		
        //use texture 1
		glActiveTexture(GL_TEXTURE0+0);
		glUniform1i(sampler0_location, 0);
		current->getSprite()->draw();
		
		//reset matrices
		worldM_ = glm::mat4();
		finalM_ = projectionM_ * viewM_ * worldM_;
	}
	
	//render position checkers of bv spawn points
	for(int i = 0; i < grid_.numBvSpawns(); ++i)
	{
		glm::vec2 coords = grid_.getBvSpawnCoords(i);
		
		// move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( coords.x, coords.y, 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		//TODO: we don't need to tint blood vessels; remove this and make new shader that doesn't use tint? 
		float tint[] = { 0.0f, 0.0f, 1.0f, 0.3f };
		glUniform4fv(inputColour_location, 1, tint);
		
        //use texture 1
		glActiveTexture(GL_TEXTURE0+0);
		glUniform1i(sampler0_location, 0);
		sprites_[0]->draw();
		
		//reset matrices
		worldM_ = glm::mat4();
		finalM_ = projectionM_ * viewM_ * worldM_;
	}
	
	//cells
	for(int i = 0; i < grid_.numCells(); ++i)
	{
		T3E::Cell* current = (T3E::Cell*)grid_.getCell(i)->getNode();
		
		// move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( grid_.getCell(i)->getX(), grid_.getCell(i)->getY(), 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		// send matrix to shaders
		glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_) );

		// set tint
		float tint[] = {current->getTint().x ,current->getTint().y , current->getTint().z, current->getTint().w};
		glUniform4fv(inputColour_location, 1, tint);

		// set texture	
        glActiveTexture(GL_TEXTURE0+1);
		
		glUniform1i(sampler0_location, 1);
				
		current->getSprite()->draw();
		
		// reset matrices
		worldM_ = glm::mat4();
		finalM_ = projectionM_*viewM_*worldM_;
		
	}
	
	//RENDER UI	
	// send ortho matrix to shaders
	glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float tint[] = {1.0f, 1.0f, 1.0f, 1.0f};	
	glUniform4fv(inputColour_location, 1, tint);
	// set texture	
	glActiveTexture(GL_TEXTURE0+2);	
	glUniform1i(sampler0_location, 2);
	//draw sprite
	bvButton_.draw();
	killButton_.draw();
		
	tintedSpriteProgram_.stopUse();	

	textRenderer_.render();

	// swap our buffers 
	window_.swapBuffer();
}

glm::vec4 MainGame::touch_to_world( glm::vec2 touch_coord )
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

    return result;
}

SDL_Point MainGame::world_to_grid( glm::vec4 world_coord )
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
	if(grid_.selectCell(row, col))
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

void MainGame::drawGrid()
{

	//draw highlighted hexes last so they're always on top of the others
	std::vector<glm::vec4> hexesInRange;
	std::vector<glm::vec4> hexesInLargeRange;
	std::vector<glm::vec4> yellowHexes;
	
	hexProgram_.use();
	for(int r = 0; r < grid_.getSize(); ++r)
	{
		for(int c = 0; c < grid_.getSize(); ++c)
		{
			glm::vec4 drawData = grid_.getHexDrawInfo(r, c, cellSelected_, selectedPos_, interactionMode_);
			//if hex exists
			if(drawData.x != -1)
			{
				//if in range of blood vessel draw it later
				if(drawData.z == 0.0f)
				{				
					hexesInRange.push_back(drawData);
				}
				else if(drawData.z == 1.0f)
				{
					hexesInLargeRange.push_back(drawData);
				}
				else if(drawData.z == 3.0f)
				{
					yellowHexes.push_back(drawData);					
				}
				else//hex is not in any range of any bv
				{
					//send matrix to shaders
					glm::mat4 transM = glm::translate(worldM_, glm::vec3(drawData.x, drawData.y, 0.0f));
					finalM_ = finalM_*transM;			
					glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
					//send range info
					glUniform1f(range_location, drawData.z);
					//set distance as 1 (lerp 1 towards neutral grid colour) since hex is out of bv range
					glUniform1f(lerp_weight_location, drawData.w);
					//set whether or not to highlight the grid
					glUniform1i(avaliable_for_highlight, 0);
					// bind the buffer object
					glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
					// tell opengl that we want to use the first attribute array
					glEnableVertexAttribArray(0);
					// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, position));
					// this is our pixel attribute pointer;
					glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, colour));
					//this is out UV attribute pointer;
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, uv));
					// draw our verticies
					glDrawArrays(GL_LINE_LOOP, 0, 6);
					// disable the vertex attrib array
					glDisableVertexAttribArray(0);
					// unbind the VBO
					glBindBuffer(GL_ARRAY_BUFFER, 0);  
					
					//reset matrix
					finalM_ = projectionM_*viewM_*worldM_;					
				}
			}
		}
	}
	
	//now draw hexes in large range
	for(std::vector<glm::vec4>::iterator data = hexesInLargeRange.begin(); data != hexesInLargeRange.end(); ++data)
	{
		//send matrix to shaders
		glm::mat4 transM = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		finalM_ = finalM_*transM;		
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//send range info
		glUniform1f(range_location, data->z);
		//send distance info
		glUniform1f(lerp_weight_location, data->w);
		//set whether or not to highlight the grid
		glUniform1i(avaliable_for_highlight, 0);
		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);
		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, uv));
		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0,6);
		// disable the vertex attrib array
		glDisableVertexAttribArray(0);
		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);  
		//reset matrix
		finalM_ = projectionM_*viewM_*worldM_;
	}
	
	//now draw hexes in range
	for(std::vector<glm::vec4>::iterator data = hexesInRange.begin(); data != hexesInRange.end(); ++data)
	{
		//send matrix to shaders
		glm::mat4 transM = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		finalM_ = finalM_*transM;		
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//send range info
		glUniform1f(range_location, data->z);
		//send distance info
		glUniform1f(lerp_weight_location, data->w);
		//set whether or not to highlight the grid
		glUniform1i(avaliable_for_highlight, 0);
		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);
		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, uv));
		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0, 6);
		// disable the vertex attrib array
		glDisableVertexAttribArray(0);
		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);  
		//reset matrix
		finalM_ = projectionM_*viewM_*worldM_;
	}
	
	//now draw yellow hexes
	for(std::vector<glm::vec4>::iterator data = yellowHexes.begin(); data != yellowHexes.end(); ++data)
	{
		//send matrix to shaders
		//translate world matrix to separate triangles and create parallax
		glm::mat4 transM = glm::translate(worldM_, glm::vec3(data->x, data->y, 0.0f));
		finalM_ = finalM_*transM;//shold be just worldM but whatever, it's a test			
		glUniformMatrix4fv(hex_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//send range info
		glUniform1f(range_location, data->z);
		//send distance info
		glUniform1f(lerp_weight_location, data->w);
		//set whether or not to highlight the grid
		glUniform1i(avaliable_for_highlight, 1);
		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, hexBufferName);
		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);
		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, uv));
		// draw our verticies
		glDrawArrays(GL_LINE_LOOP, 0, 6);
		// disable the vertex attrib array
		glDisableVertexAttribArray(0);
		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);  
		//reset matrix
		finalM_ = projectionM_*viewM_*worldM_;
	}
	
	hexProgram_.stopUse();
}
