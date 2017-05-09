// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"




// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <stdlib.h>


#include <stdexcept>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <memory>
#include <array>
#include <functional>
#include <bitset>


#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <rapidxml\rapidxml.hpp>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <entityx\entityx.h>

#include <btBulletDynamicsCommon.h>

#include "myRandom.h"
#include "utils.h"
#include "Physics.h"
#include "Input.h"
#include "Camera.h"
#include "Shaders.h"

