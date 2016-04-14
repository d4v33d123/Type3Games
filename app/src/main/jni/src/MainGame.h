#ifndef MAINGAME_H
#define MAINGAME_H

//SDL includes
#include "SDL.h"
//OpenGL ES includes
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif  //__ANDROID__
//GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//STL includes
#include <string>
#include <vector>
//Type3Engine includes
#include "Type3Engine/Sprite.h"
#include "Type3Engine/GLSLProgram.h"
#include "Type3Engine/glTexture.h"
#include "Type3Engine/window.h"
#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/errors.h"
#include "Type3Engine/Camera.h"
#include "Type3Engine/AudioEngine.h"
#include "Type3Engine/Button.h"
#include "Type3Engine/TextRenderer.h"
//game classes
#include "Cell.h"
#include "BloodVessel.h"
#include "Grid.h"
#include "command.h"
#include "Type3Engine/Vertex.h"//draw grid

enum class GameState { PLAY, EXIT };


class MainGame
{
public:
	MainGame();
	~MainGame();

	command run(T3E::window* window, T3E::AudioEngine* audioEngine);

private:
	//camera sensitivity
	const int PAN_SENSITIVITY;
	const int ZOOM_SENSITIVITY;
	// Debug Cursor
    glm::vec4 cursor_pos_;
	//glm::vec4 cursor_pos_on_fup_;

	//control functions
	void initSystems();
	void initShaders();
	command gameLoop();
	void processInput(float dTime);
	void renderGame();
	void calculateFPS();
	
	//control vars
	float fps_;
	float frameTime_;
	float maxFPS_;
	T3E::window* window_;
	GameState gameState_;
	InteractionMode interactionMode_;
	
    T3E::Grid grid_;

	//GAMEPLAY
	Uint8 nOfFingers_;                  // n of fingers currently touching screen
	T3E::Camera camera_;                // 2d camera
	std::vector<T3E::Sprite*> sprites_; // sprite container TODO: remove this when bv sprite is in right place. maybe use for ui or smt
	int score_;							// the player's score
	bool paused_;
	bool gameOver_;
	
	// [in] row to test
	// [in] column to test
	// [ret] true if successfully selected a cell, false otherwise
	bool selectCell(int row, int col);
	
    //INPUT
	
	T3E::Button bvButton_, killButton_, menuButton_, resumeButton_, quitButton_;
	bool finger_dragged_;
	//detect when finger is down for a certain amount of time
	bool fingerPressed_;
	glm::vec2 pressPos_;
	float pressTimer_;
	//detect cell selection
	glm::vec2 selectedPos_;
	bool cellSelected_;
	//interaction mode
	bool bvCreationMode_;
	
	//GRAPHICS
	T3E::GLSLProgram tintedSpriteProgram_;//shader programs
	GLint cell_finalM_location, sampler0_location, inputColour_location; // shader uniform locations
	glm::mat4 worldM_, viewM_, projectionM_, viewProjInverse, orthoM_; // transform matrices
	glm::mat4 finalM_; // product of above 3, do in cpu once per geometry vs do in gpu once per each vertex(profile this?)
	bool avaliable_for_highlight;
	T3E::Sprite backgroundSprite_;
	T3E::TextRenderer textRenderer_;
	
	//AUDIO
    T3E::AudioEngine* audioEngine_;
	T3E::SoundEffect bloodV_;
	T3E::SoundEffect cellDeath_;
	T3E::SoundEffect cellArrest_;
	T3E::SoundEffect cellModeChange_;
	T3E::SoundEffect select_;

	// Conversion Functions
    // Returns a vec4 where x and y are the touch world positions, z is 0.0f, w is a number
    glm::vec4 touch_to_world( glm::vec2 touch_coord );
    // Returns an SDL_Point where x represents the row and y represents the column
    SDL_Point world_to_grid( glm::vec4 world_coord );
	
	//Draw hex grid 	
	T3E::GLSLProgram hexProgram_;
	GLint hex_finalM_location, range_location, lerp_weight_location;
 	GLuint hexBufferName;

	void drawGrid();
};

#endif
