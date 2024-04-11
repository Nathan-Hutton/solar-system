// For now, this file is just meant to let us toggle between a shader program
// that uses shadows, and one that doesn't, with a key press. The purpose of this is so that we can call
// this method (which changes global variables defined in main.cpp) from the Camera class,
// which handles inputs.

#ifndef SHADERHELPERFUNCTIONS_H
#define SHADERHELPERFUNCTIONS_H

void toggleShadows();

#endif
