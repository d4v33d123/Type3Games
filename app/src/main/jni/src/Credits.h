#ifndef CREDITS_H
#define CREDITS_H

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
//#include "Type3Engine/errors.h"
#include "Type3Engine/AudioEngine.h"
#include "Type3Engine/Button.h"

#include "command.h"
#include "Cell.h"
#include "Grid.h"
#include "BloodVessel.h"
class Credits
{
public:
	command run(T3E::window* window, T3E::AudioEngine* audioEngine);

private:
	//control functions
	void initSystems();
	void initShaders();
	command gameLoop();
	command processInput();
	void renderGame();
	void calculateFPS();

	T3E::window* window_;
	float fps_;
	float frameTime_;
	float maxFPS_;
    
	//INPUT
	T3E::Button backButton_;
	
	//GRAPHICS
	T3E::GLSLProgram tintedSpriteProgram_;//shader programs
	GLint tintedSprite_finalM_location, sampler0_location, inputColour_location;//shader uniform locations
	glm::mat4 orthoM_;//transform matrices
	T3E::Sprite backgroundSprite_;
	
	//AUDIO
    T3E::AudioEngine* audioEngine_;	
	T3E::SoundEffect buttonPress_;
};

#endif
