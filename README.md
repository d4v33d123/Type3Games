# Android version

## Change Log of the first commit:

### Changes from Desktop GLES test project (Visual studio project)
- changed #pragma once with header guards (#pragma directives not supported by ndk compiler(?))
- changed #include <SDL/SDL.h> to "SDL.h" (sdl path specified in mk file)
- removed glesloader.h and changed its include instances with #include <GLES2/gl2.h>, #include <GLES2/gl2ext.h>
- removed LOAD_OPENGL_ES_FUNCTIONS macro in functions (a macro I used to not have that horrible wall of text everywhere)
- added #ifdef \_\_ANDROID\_\_ preprocessor macro where including things for Android only (OpenGLES stuff, stddef.h ndk header)

### Changes from Android GLES test project
- changed manifest and icon pngs to display t3icon and app name in launcher
- changed manifest to force landscape screen orientatioin
- added APP_STL := gnustl_static in application.mk to enable latest stl functionality
- added LOCAL_CFLAGS := -std=c++11 in android.mk to enable c++11
- added assets folder to store shaders and textures

### Changes to engine
- added destroy() in window class and call to SDL_Quit() in mainGame.cpp to properly close the app
- defined our own to_str function in ImageLoader.cpp cause compiler says 'to_string' is not a member of 'std'
- using SDL_Log instead of iostream and printf
- rewrote IOManager to use SDL_RWops instead of fstream and split into 2 functions to read binary and text files respectively(read binary for loading textures, txt for loading shader sources)
- modified CompileShader func in GLSLProgram to use new IOManager functionality
- included ndk's stddef.h in picoPNG(see app/src/main/jni/NDK/whyIsThisHere.txt)
- TEMP: texture cache now always loads the texture cause was giving problem
- added GLM
- added updateSizeInfo() func to window class to make width and height vars useful
- remapped uv coords in sprite class so texture is upright (I think the decodepng function is loading imgs 90° rotated to the left and mirrored)
- changed vertex colour of sprite mesh to white

### Changes to MainGame
- changed include paths to match new structure
- added world view and projection transform matrices; should put them into class to manage transform/resets etc
- enabled back face culling
- added test code for touch input (pinch to zoom, slide to pan) and "back" button. to be finished/cleaned up and put into camera/input class
- added 2 shaders, 1 draws a tricolour triangle, the other draws a sprite with transparency and blends sprite colour with input colour(which is given based on camera displacement)

### Bugs / todo
- When finger up or finger down is detected, the world coordinates of the touch are printed to the command line, but I think the calculation is wrong when the camera is not at the original position.
- Is fps calculation/limiting working? sometimes it says 4k fps
- Check texture cache makes sense
- Look into using frame buffer fetch for blending instead of glEnable(GL_BLEND)
- unbinding vertex info buffers after use could be a useless loss of performance(seems so in modern desktop opengl, check for es2.0)
- consider changing near/far clipping planes to improve performance since we don't really need much depth in a 2d game
- new to string function is used just once, no need to import sstream just for that, find workaround


