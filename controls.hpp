//copied from <common/controls.hpp>
#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <GLFW/glfw3.h>
void computeMatricesFromInputs2(GLFWwindow* hWindow);
glm::mat4 getViewMatrix2();
glm::mat4 getProjectionMatrix2();
mat4 getObjectMoveMat();

#endif
