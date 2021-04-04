#ifndef SHADER_HPP
#define SHADER_HPP

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif

/*
 * File Name: shader.h
 *
 * Contains include statements which will be used in other files. Also defines
 * methods needed to perform OpenGL shader setup.
 *
 * @author Part of 169 starter code
 */