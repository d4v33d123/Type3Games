LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
GLM_PATH := ../GLM
SDL_MIXER_PATH := ../SDL_MIXER
# here ndk path refers to a few ndk compiler specific headers, not the location of the ndk folder
NDK_PATH := ../NDK
#WWISE_PATH := ../WWISE/include

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(GLM_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(NDK_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_MIXER_PATH)
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(WWISE_PATH)

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	Type3Engine/picoPNG.cpp \
	Type3Engine/errors.cpp \
	Type3Engine/GLSLProgram.cpp \
	Type3Engine/ImageLoader.cpp \
	Type3Engine/IOManager.cpp \
	Type3Engine/ResourceManager.cpp \
	Type3Engine/Sprite.cpp \
	Type3Engine/AnimatedSprite.cpp \
	Type3Engine/TextureCache.cpp \
	Type3Engine/Type3Engine.cpp \
	Type3Engine/AudioEngine.cpp \
	Type3Engine/window.cpp \
	Type3Engine/Camera.cpp \
	Hex.cpp \
	BloodVessel.cpp \
	Cell.cpp \
	MainGame.cpp \
	main.cpp \
	Grid.cpp \
	Node.cpp
	
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

LOCAL_CFLAGS := -std=c++11

include $(BUILD_SHARED_LIBRARY)
