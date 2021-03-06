#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>


#include <stdlib.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#pragma once

#include "Window.h"
#include <irrKlang.h>
//
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
#endif

/*
 * File Name: main.h
 *
 * Contains include statements which will be used in other files.
 *
 * @author Part of 169 starter code
 */

