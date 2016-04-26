#include "Tutorial.h"

Tutorial::Tutorial() :
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(6.0f),
	maxFPS_(60.0f)
{
}

Tutorial::~Tutorial()
{
}

command Tutorial::run(T3E::window* window, T3E::AudioEngine* audioEngine)
{
	window_ = window;
	audioEngine_ = audioEngine;
	
	initSystems();
	
	buttonPress_ = audioEngine_->loadSoundEffect("sound/Player_CellDivide_Move.ogg");
	
	command result = gameLoop();
	
	return result;
}

void Tutorial::initSystems()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glClearDepthf(1.0);

	window_->updateSizeInfo();
	float ratio = window_->getScreenWidth() / float( window_->getScreenHeight() );	// calculate aspect ratio
	orthoM_ = glm::ortho(0.0f, float( window_->getScreenWidth() ), 0.0f, float( window_->getScreenHeight() ));
    projection_martix_ = glm::perspective( 90.0f, ratio, 0.1f, 100.0f );

	// init camera at 0,0,1 looking at origin, up is y axis
	camera_.init( glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f,0.0f,0.0f ) );
	camera_.setZoomRange( glm::vec2(1.0f, 8.0f) );
	camera_.setSensitivity( PAN_SENSITIVITY, ZOOM_SENSITIVITY );
	camera_.moveTo( glm::vec3( 0.0f, 0.0f, 5.0f ) );

	backButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(1.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		4.0f/16, 2.0f/4,
		4.0f/16, 3/4.0f);
	
	//background sprite
	backgroundSprite_.init(0.0f, 0.0f, float(window_->getScreenWidth()), float(window_->getScreenHeight()),"textures/background.png", 0, 0, 1.0f, 1.0f);
	
	textRenderer_.init();
	textRenderer_.setScreenSize( window_->getScreenWidth(), window_->getScreenHeight() );

	initGridVerts();

	// Set a test blood vessel
    grid_.newBloodVessel( 24, 24, nullptr );

	//init shaders
	initShaders();
}

void Tutorial::initGridVerts()
{
	//init the hex vertex buffer
	glGenBuffers(1, &hex_buffer_name_);
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

	glBindBuffer(GL_ARRAY_BUFFER, hex_buffer_name_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * hexVerts.size(), hexVerts.data(), GL_STATIC_DRAW);
}

void Tutorial::initShaders()
{
	//CELL PRORGAM
	tintedSpriteProgram_.compileShaders("shaders/tintedSprite_vs.txt", "shaders/tintedSprite_ps.txt");
	tintedSpriteProgram_.addAttribute("aPosition");
	tintedSpriteProgram_.addAttribute("aTexCoord");
	tintedSpriteProgram_.linkShaders();

	// query uniform locations - could use "layout location" in shaders to set fixed locations
	tintedSprite_finalM_location = tintedSpriteProgram_.getUniformLocation("finalM");
	sampler0_location = tintedSpriteProgram_.getUniformLocation("sampler0");
	inputColour_location = tintedSpriteProgram_.getUniformLocation("inputColour");

	// HEX SHADER This shader draws the grid
	hexProgram_.compileShaders("shaders/hex_vs.txt", "shaders/hex_ps.txt");
	hexProgram_.addAttribute("aPosition");
	hexProgram_.linkShaders();

	// query uniform locations - could use "layout location" in shaders to set fixed locations
	range_location_ = hexProgram_.getUniformLocation("range");
	lerp_weight_location_ = hexProgram_.getUniformLocation("weight");
	avaliable_for_highlight_ = hexProgram_.getUniformLocation("Avaliable");
	hex_finalM_location_ = hexProgram_.getUniformLocation("finalM");	
}

command Tutorial::gameLoop()
{	
	command c;
	Uint32 old_ticks = SDL_GetTicks();
	Uint32 ticks = SDL_GetTicks();
	
	while((c = processInput()) == command::NONE)
	{
		textRenderer_.putString(
"\
- Drag to move around the screen\n\
- Pinch to zoom in and out\n\
- In the top left is a currency and score meter\n\
- Currency rises and falls as you do different\n\
  actions in game, score goes up as you grow\n\
- Stem cells can be moved and split manualy,\n\
  over time they naturaly create healthy cells\n\
- To Split a stem cell, hold down on it untill\n\
  it changes colour, than tap and adjacent hex\n\
- To create a blood vessel first tap the\n\
  blood vessel button on the left. Then hold on\n\
  the grid where you would like to create one.\n\
- Once a blood vessel marker has been placed,\n\
  move a stem cell into the centre and fill the\n\
  edges with healthy cells to create it\n\
- Over time healthy cells will start to mutate,\n\
  use the kill button on the left to remove mutated\n\
  cells before they mutate even more and stop\n\
  responding to the kill command!\n\
- Holding down on a healthy cell will arrest it,\n\
  arrested cells are usefull for segregating\n\
  dangerously mutated cells and preventing them\n\
  from spreading too far\n\
  \n\
  TODO: interactive tutorial\n\
", -0.98, 0.98, 38
);

		renderGame();
		
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > ticks - old_ticks) {
			SDL_Delay(1000.0f / maxFPS_ - (ticks - old_ticks));
		}

		old_ticks = ticks;
		ticks = SDL_GetTicks();
	}
	
	return c;
}

command Tutorial::processInput()
{
	glm::vec2 screenCoords;
	
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch( evnt.type )
		{
		case SDL_QUIT:
			return command::QUIT;
			break;
		
		case SDL_FINGERDOWN:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			screenCoords = glm::vec2(evnt.tfinger.x * float(window_->getScreenWidth()),
				float(window_->getScreenHeight()) - evnt.tfinger.y * float(window_->getScreenHeight()));
				
			if( backButton_.touchCollides(screenCoords) )
			{
				backButton_.press();
			}
			return command::NONE;
			break;
			
		case SDL_FINGERUP:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			screenCoords = glm::vec2(evnt.tfinger.x * float(window_->getScreenWidth()),
				float(window_->getScreenHeight()) - evnt.tfinger.y * float(window_->getScreenHeight()));
				
			if(backButton_.touchCollides(screenCoords))
			{
				backButton_.unpress();
				return command::MENU;	
			}
			return command::NONE;
			break;
			
		default:
			return command::NONE;
		}
	}

	return command::NONE;
}

void Tutorial::renderGame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view_martix = glm::lookAt(camera_.getPosition(), camera_.getLookAt(), camera_.getUp());
	view_proj_martix_ = view_martix * projection_martix_;
	
	tintedSpriteProgram_.use();
	
	//RENDER BACKGROUND
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float white_tint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv(inputColour_location, 1, white_tint);
	// set texture	
	GLint texid = T3E::ResourceManager::getTexture("textures/background.png").unit;
	glActiveTexture(GL_TEXTURE0 + texid);	
	glUniform1i(sampler0_location, texid);
	backgroundSprite_.draw();
	
	drawGrid();

	tintedSpriteProgram_.use();

	//RENDER UI	
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	glUniform4fv(inputColour_location, 1, white_tint);
	// set texture
	// texid = T3E::ResourceManager::getTexture("textures/ui2.png").unit;
	// glActiveTexture(GL_TEXTURE0 + texid);	
	// glUniform1i(sampler0_location, texid);
	//draw sprite
	backButton_.draw();

	textRenderer_.render();

	// swap our buffers 
	window_->swapBuffer();
}

void Tutorial::drawGrid()
{
	hexProgram_.use();

	// TODO: replace this shitty excure for an enum
	// The getHexDrawInfo function should really be redsigned
	static const float CLOSE_RANGE_OF_BV = 0.0f;
	static const float FAR_RANGE_OF_BV = 1.0f;
	static const float YELLOW = 3.0f;

	// Send the verts to the gpu and specify the layout
	glBindBuffer(GL_ARRAY_BUFFER, hex_buffer_name_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
					
	//send range info
	glUniform1f(range_location_, 2.0f );
	//set whether or not to highlight the grid
	glUniform1i(avaliable_for_highlight_, 0);

	for(int r = 0; r < grid_.getSize(); ++r)
	{
		for(int c = 0; c < grid_.getSize(); ++c)
		{
			T3E::Hex* hex;
			//grid_.getHex( r, c, &hex );
			//glm::vec4 drawData = grid_.getHexDrawInfo(r, c, cellSelected_, selectedPos_, interactionMode_);

			//send matrix to shaders
			glm::mat4 tranlation_matrix = glm::translate( glm::mat4(), glm::vec3( 1.0f, 1.0f, 0.0f));
			glm::mat4 final_matrix = view_proj_martix_ * tranlation_matrix;			
			glUniformMatrix4fv(hex_finalM_location_, 1, GL_FALSE, glm::value_ptr(final_matrix));
			
			glDrawArrays(GL_LINE_STRIP, 0, 4);
			
			// Fill in the gaps at the edges
			if( r == 0 && c < grid_.getSize() - 1 )
				glDrawArrays( GL_LINES, 3, 2 );
			if( r == grid_.getSize() - 1 )
				glDrawArrays( GL_LINES, 5, 2);
			if( c == grid_.getSize() - 1 )
				glDrawArrays( GL_LINE_STRIP, 3, 3 );

			/*
			//glm::vec4 drawData = grid_.getHexDrawInfo(r, c, cellSelected_, selectedPos_, interactionMode_);
			glm::vec4 drawData = grid_.getHexDrawInfo(r, c, false, glm::vec2(), InteractionMode::NORMAL);
			drawData.z = 2.0f;

			//glm::vec4 drawData = glm::vec4(1.0f, 1.0f, 1.0f, 2.0f);
			//if hex exists
			if(drawData.x != -1)
			{
				//if in range of blood vessel draw it later
				if(drawData.z == CLOSE_RANGE_OF_BV )
				{				
					//hexesInRange.push_back(drawData);
				}
				else if(drawData.z == FAR_RANGE_OF_BV )
				{
					//hexesInLargeRange.push_back(drawData);
				}
				else if(drawData.z == YELLOW )
				{
					//yellowHexes.push_back(drawData);					
				}
				else // hex is not in any range of any bv, draw it normaly
				{
					//send matrix to shaders
					glm::mat4 tranlation_matrix = glm::translate( glm::mat4(1.0), glm::vec3(drawData.x, drawData.y, 0.0f));
					glm::mat4 final_matrix = view_proj_martix_ * tranlation_matrix;			
					glUniformMatrix4fv(hex_finalM_location_, 1, GL_FALSE, glm::value_ptr(final_matrix));
					
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
			*/
		}
	}	
}

void Tutorial::calculateFPS()
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