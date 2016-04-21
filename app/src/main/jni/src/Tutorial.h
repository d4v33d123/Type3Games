#ifndef TUTORIAL_H
#define TUTORIAL_H

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
//#include "Type3Engine/glTexture.h"
#include "Type3Engine/window.h"
#include "Type3Engine/Camera.h"
//#include "Type3Engine/errors.h"
#include "Type3Engine/AudioEngine.h"
#include "Type3Engine/Button.h"
#include "Type3Engine/TextRenderer.h"

#include "command.h"
#include "Cell.h"
#include "Grid.h"
#include "BloodVessel.h"
class Tutorial
{
public:
	Tutorial();
	~Tutorial();
	command run(T3E::window* window, T3E::AudioEngine* audioEngine);

private:
	const int PAN_SENSITIVITY;
	const int ZOOM_SENSITIVITY;
	
	//control functions
	void initSystems();
	void initShaders();
	void initGridVerts();
	command gameLoop();
	command processInput();
	void renderGame();
	void calculateFPS();
	void drawGrid();

	T3E::TextRenderer textRenderer_;
	T3E::window* window_;
	T3E::Camera camera_;
	T3E::Grid grid_;
	
	float fps_;
	float frameTime_;
	float maxFPS_;
    
	//INPUT
	T3E::Button backButton_;
	
	//GRAPHICS
	T3E::GLSLProgram tintedSpriteProgram_;
	GLint tintedSprite_finalM_location, sampler0_location, inputColour_location; // shader uniform locations
	glm::mat4 projection_martix_;
	glm::mat4 view_proj_martix_; // The product of the view and projection marticies

	T3E::GLSLProgram hexProgram_;
	GLint hex_finalM_location_, range_location_, lerp_weight_location_, avaliable_for_highlight_;
 	GLuint hex_buffer_name_;

	glm::mat4 orthoM_; // transform matrices
	T3E::Sprite backgroundSprite_;
	
	//AUDIO
    T3E::AudioEngine* audioEngine_;	
	T3E::SoundEffect buttonPress_;
};

#endif
