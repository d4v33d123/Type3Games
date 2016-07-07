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
#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/AudioEngine.h"
#include "Type3Engine/errors.h"
#include "Type3Engine/Camera.h"
#include "Type3Engine/Button.h"
#include "Type3Engine/Timer.h"
//game classes
#include "Cell.h"
#include "BloodVessel.h"
#include "Grid.h"
#include "command.h"
#include "Type3Engine/Vertex.h"//draw grid

/// The game is either playing or request quit
enum class GameState { PLAY, EXIT };

// Forward declare pointer member classes
namespace T3E
{
	class TextRenderer;
	class window;
}

class MainGame
{
public:
	MainGame();
	~MainGame();

	/**
	* @param [in] Window* The window to render to
	* @param [in] AudioEngine* The audio engine to use
	* @param [in] Bool flag of weather to run the game with tutorial hints on	
	* Run handles initialisation of the state then enters the game loop
	*/
	command run(T3E::window* window, T3E::AudioEngine* audioEngine, T3E::TextRenderer* textRenderer, bool tutorial);

private:
	//camera sensitivity
	const int PAN_SENSITIVITY;
	const int ZOOM_SENSITIVITY;

	//control functions
	void initSystems();
	void initShaders();
	command gameLoop();
	void processInput(float dTime);
	void renderTutorial();				///< Handles updating and drawing of the tutorial
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

	// GAMEPLAY
	Uint8 nOfFingers_;                  // n of fingers currently touching screen
	T3E::Camera camera_;                // 2d camera
	std::vector<T3E::Sprite*> sprites_; // sprite container TODO: remove this when bv sprite is in right place. maybe use for ui or smt
	int score_;							// the player's score
	bool paused_;
	bool gameOver_;

	// TUTORIAL
	bool tutorial_;
	TutorialPhase tut_phase_;
	T3E::Timer timer_;
	void increment_tutorial();
	
	/**
	* @param [in] row to test
	* @param [in] column to test
	* @return true if successfully selected a cell, false otherwise
	*/
	bool selectCell(int row, int col);
	
    // INPUT
    T3E::GLSLProgram uiProgram_;
    GLint ui_sampler_location_, ui_finalM_location_;
	T3E::Button bvButton_, killButton_, menuButton_, resumeButton_, quitButton_;
    T3E::Button scorebar_;
    T3E::Button nextButton_;	///< Increments the tutorial
	
	bool finger_dragged_;	///< True when a finger is being dragged significantly
	bool finger_down_;		///< True when a finger is down, false when finger dragged of no fingers are down
	bool finger_pressed_;	///< True for the first frame of a finger touching the screen
	bool finger_lifted_;	///< True for the first frame of a finger being lifted off the screen
	glm::vec2 finger_position_sdl_;			///< SDL coordinates, normalised from (0, 1)
	glm::vec2 finger_position_world_;		///< OpenGL world coords
	SDL_Point finger_position_pixels_;		///< The pixel position of the touch
	SDL_Point finger_position_row_col_;		///< The row (x) and column (y) of the finger on the grid
	glm::vec2 finger_down_position_sdl_;	///< Where the finger was put down on the screen

	float pressTimer_;

	//detect cell selection
	glm::vec2 selectedPos_;
	bool cellSelected_;

	//interaction mode
	bool bvCreationMode_;
	
	// GRAPHICS
	T3E::GLSLProgram tintedSpriteProgram_;
	GLint cell_finalM_location, sampler0_location, inputColour_location; // shader uniform locations
	glm::mat4 worldM_, viewM_, projectionM_, viewProjInverse, orthoM_; // transform matrices
	glm::mat4 finalM_; // product of above 3, do in cpu once per geometry vs do in gpu once per each vertex. TODO: profile this?
	bool avaliable_for_highlight;
	T3E::Sprite backgroundSprite_;
	T3E::TextRenderer* textRenderer_;
	
	// AUDIO
    T3E::AudioEngine* audioEngine_;
	T3E::SoundEffect blood_vessel_sound_;
	T3E::SoundEffect cell_death_sound_;
	T3E::SoundEffect cell_arrest_sound_;
	T3E::SoundEffect cell_mode_change_sound_;
	T3E::SoundEffect select_sound_;

	void update_finger_position( float x, float y);

	// Conversion Functions
    // Returns a vec2 where x and y are the touch world positions
    glm::vec2 touch_to_world( glm::vec2 touch_coord );
    // Returns the pixel position of the touch
    SDL_Point touch_to_pixels( glm::vec2 touch_coord );
    // Returns an SDL_Point where x represents the row and y represents the column
    SDL_Point world_to_grid( glm::vec2 world_coord );
	
	//Draw hex grid 	
	T3E::GLSLProgram hexProgram_;
	GLint hex_finalM_location, range_location, lerp_weight_location;
 	GLuint hexBufferName;

	void drawGrid();
};

#endif
