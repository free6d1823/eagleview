#include <GL/glew.h>
//GLFW first
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



using namespace glm;
////////////////
#include "car.h"
#include "calibration.h"

#include "controls.hpp"
#include <stdio.h>

extern bool	g_bShowCar;
extern Car   g_car;

enum ViewMode {
    VM_NONE = 0,
    VM_BIRDVIEW = 1,    //see from top
    VM_BEHIND = 2,      //see behind car
    VM_AHEAD = 3,       //see in front of car
    VM_FRONT = 4,       //sit inside car see front
    VM_BACK = 5,        // see back
    VM_LEFT = 6,        // see left side
    VM_RIGHT = 7,       //see right side
    VM_SAT = 8,       //satellite patrol
};
ViewMode m_eCurrentMode = VM_BIRDVIEW;
ViewMode m_eLastMode = VM_NONE;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix2(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix2(){
	return ProjectionMatrix;
}

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14/2.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 55.0f;
float radius = 5.0f;
////////////////////////////////////////////
glm::vec3 posCam = glm::vec3( 0, 0, 5 ); 
glm::vec3 posLookAt = glm::vec3( 0, 0, 0 );
glm::vec3 camUp = glm::vec3( 0, 1, 0 );
float m_FoV = 55;
///////////////////////////////////////////

mat4 ObjectMatrix = mat4(0);
mat4 getObjectMoveMat()
{
    return ObjectMatrix;
}
/* process key definition VM_BIRDVIEW */
void doBirdView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = 60;
        posCam.x = 0;
        posCam.y = SPHERE_RADIUS;
        posCam.z = 0;
        posLookAt = vec3(0,0,0);
        camUp = vec3( 0, 0, -1 );
    }


	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		posCam.y -= BIRDVIEW_STEP_ZOOM;
		if(posCam.y < BIRDVIEW_MIN_ZOOM) posCam.y = BIRDVIEW_MIN_ZOOM;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		posCam.y += BIRDVIEW_STEP_ZOOM;
		if(posCam.y > BIRDVIEW_MAX_ZOOM) posCam.y = BIRDVIEW_MAX_ZOOM;
	}

    float rotateSpeed = BIRDVIEW_ROTATE_STEP;
    float ca = cos(rotateSpeed);
    float sa = sin(rotateSpeed);
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		camUp = camUp * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		camUp = camUp * mat3(ca, 0, -sa, 0,1,0, sa, 0, ca);
	}

}
/* process key definition VM_BEHIND */
void doBehindView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = BEHIND_FOV;
        posCam.x = 0;
        posCam.y = (CAR_HEIGHT*2.5);
        posCam.z = (CAR_HEIGHT*2.5);
        camUp = vec3( 0, 1, 0 );
    }
    posLookAt = (vec3) g_car.getPos();
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		posCam.z -= BEHIND_STEP_ZOOM;
		if(posCam.z < posLookAt.z + BEHIND_MIN_ZOOM)posCam.z = posLookAt.z + BEHIND_MIN_ZOOM;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		posCam.z += BEHIND_STEP_ZOOM;
		if(posCam.z > posLookAt.z + BEHIND_MAX_ZOOM)posCam.z = posLookAt.z + BEHIND_MAX_ZOOM;
	}
	// Strafe right
    float ca = cos(BEHIND_ROTATE_STEP);
    float sa = sin(BEHIND_ROTATE_STEP);
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		posCam = posCam * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		posCam = posCam * mat3(ca, 0, -sa, 0,1,0, sa, 0, ca);
	}
	//printf("current cam (%f, %f, %f)\n", posCam.x, posCam.y, posCam.z);
}
/* process key definition VM_AHEAD */
void doAheadView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = AHEAD_FOV;
        posCam.x = 0;
        posCam.y = (CAR_HEIGHT*2.5);;
        posCam.z = -(CAR_HEIGHT*2.5);;
        camUp = vec3( 0, 1, 0 );
    }
    posLookAt = (vec3) g_car.getPos();
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		posCam.z -= AHEAD_STEP_ZOOM;
		if(posCam.z < posLookAt.z + AHEAD_MIN_ZOOM)posCam.z = posLookAt.z + AHEAD_MIN_ZOOM;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		posCam.z += AHEAD_STEP_ZOOM;
		if(posCam.z > posLookAt.z + AHEAD_MAX_ZOOM)posCam.z = posLookAt.z + AHEAD_MAX_ZOOM;
	}
	// Strafe right
    float ca = cos(AHEAD_ROTATE_STEP);
    float sa = sin(AHEAD_ROTATE_STEP);
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		posCam = posCam * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		posCam = posCam * mat3(ca, 0, -sa, 0,1,0, sa, 0, ca);
	}
	//printf("current cam (%f, %f, %f)\n", posCam.x, posCam.y, posCam.z);
}
/* process key definition VM_BACK */
void doBackView(GLFWwindow* hWindow) {
	static float fUp = 0;
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = BACK_FOV;
        camUp = vec3( 0, 1, 0 );
        posLookAt.y = CAR_HEIGHT;
    }
    posCam = (vec3) g_car.getPos();
    posCam.y = CAR_HEIGHT*1.4; //rear mirror position
    posLookAt.x = g_car.getDir().x * 20;
    posLookAt.z = g_car.getDir().z * 20;
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		fUp += 0.02;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		fUp -= 0.02;
		if (fUp <0 ) fUp = 0;
	}
    posCam.z += fUp;
	// Strafe right
    static float rotateSpeed = 0;
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		rotateSpeed += 0.02;
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		rotateSpeed -= 0.02;
	}
    float ca = cos(rotateSpeed);
    float sa = sin(rotateSpeed);
	posLookAt = posLookAt * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
}

/* process key definition VM_FRONT */
void doFrontView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = 80;
        posLookAt.y = CAR_HEIGHT;
        camUp = vec3( 0, 1, 0 );
    }
	static float fUp = 0;
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		fUp -= 0.1;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		fUp += 0.1;
	}

    posCam = (vec3) g_car.getPos();
    posCam.z += fUp;
    posCam.y = CAR_HEIGHT*1.4; //rear mirror position
    posCam.x -= 0.5;
//    posCam.z -= 0.5;
    posLookAt.x = g_car.getDir().x * 20;
    posLookAt.z = -g_car.getDir().z * 20;


    static float rotateSpeed = 0;
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		rotateSpeed += 0.02;
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		rotateSpeed -= 0.02;
	}
    float ca = cos(rotateSpeed);
    float sa = sin(rotateSpeed);
	posLookAt = posLookAt * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
}
/* process key definition VM_LEFT */
void doLeftView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = 80;
        posCam.x = 0.5;
        posCam.y = 3;
        posCam.z = 0;
        posLookAt.x = 5;
        posLookAt.y = 3.0;
        posLookAt.z = 0;
        camUp = vec3( 0, 1, 0 );
    }
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		posCam.x -= 0.1;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		posCam.x += 0.1;
	}
	// Strafe right
    float rotateSpeed = 0.02;
    float ca = cos(rotateSpeed);
    float sa = sin(rotateSpeed);
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		posLookAt = posLookAt * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		posLookAt = posLookAt * mat3(ca, 0, -sa, 0,1,0, sa, 0, ca);
	}
}
/* process key definition VM_RIGHT */
void doRightView(GLFWwindow* hWindow) {
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = 80;
        posCam.x = -0.5;
        posCam.y = 3;
        posCam.z = 0;
        posLookAt.x = -5;
        posLookAt.y = 3.0;
        posLookAt.z = 0;
        camUp = vec3( 0, 1, 0 );
    }
	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		posCam.x -= 0.1;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		posCam.x += 0.1;
	}
	// Strafe right
    float rotateSpeed = 0.02;
    float ca = cos(rotateSpeed);
    float sa = sin(rotateSpeed);
    //rotate with Y
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		posLookAt = posLookAt * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		posLookAt = posLookAt * mat3(ca, 0, -sa, 0,1,0, sa, 0, ca);
	}
}
void doSatView(GLFWwindow* hWindow) {
	static float theda = 0; //this is moving with time
	static float phi = 35*PI/180; //adjust by UP/DOWN
	static float speed = 0.001;// theda variation, adjusted by LEFT/RIGHT

    posLookAt = (vec3) g_car.getPos();
    if(m_eLastMode != m_eCurrentMode) {
        m_eLastMode = m_eCurrentMode;
        m_FoV = 55;
        camUp = vec3( 0, 1, 0 );
        speed = 0.001;
    }

	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP  ) == GLFW_PRESS){
		phi += 0.005;
		if (phi> 85*PI/180) phi = 85*PI/180;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		phi -= 0.005;
		if( phi < 30*PI/180) phi =30*PI/180;
	}
	if (glfwGetKey( hWindow, GLFW_KEY_LEFT  ) == GLFW_PRESS){
		speed+= 0.0005;
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		speed-= 0.0005;
	}
	theda += speed;
	if(theda > 2*PI) theda -= 2*PI;
	if(theda < 0)theda+= 2*PI;

    float cx = cos(phi);
    float sx = sin(phi);
    float cy = cos(theda);
    float sy = sin(theda);

    posCam = vec3(0,0, SPHERE_RADIUS*0.85) *
    		mat3(1,0,0, 0, cx, sx, 0, -sx, cx)* mat3(cy, 0, sy, 0,1,0, -sy, 0, cy);
printf("Cam pos(%f,%f,%f)\n", posCam.x, posCam.y, posCam.z);

}
void doSatView2(GLFWwindow* hWindow) {
	static float speed = 0.001;
	static float angle = 15*3.14/180;
	static float radius = 10;

    posLookAt = (vec3) g_car.getPos();
    if(m_eLastMode != m_eCurrentMode) {
        //init this mode
        m_eLastMode = m_eCurrentMode;
        m_FoV = 60;
        //posCam.x = 0;
        //posCam.y = posLookAt.y;
        //posCam.z = radius;
        camUp = vec3( 0, 1, 0 );
        speed = 0.001;
    }

	// Move forward
	if (glfwGetKey( hWindow, GLFW_KEY_UP ) == GLFW_PRESS){
		angle += 0.01;
	}
	// Move backward
	else if (glfwGetKey( hWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
		angle -= 0.01;
		if( angle < 0) angle =0;
	}
    float ca = cos(angle);
    float sa = sin(angle);
    posCam.y = posLookAt.y + radius* sa;
    posCam.z = posLookAt.z + radius* ca;
    //rotate with X
	if (glfwGetKey( hWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		speed+= 0.0005;
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
		speed-= 0.0005;
	}
	//rotate Y
    ca = cos(speed);
    sa = sin(speed);
	posCam = posCam * mat3(ca, 0, sa, 0,1,0, -sa, 0, ca);
printf("Cam pos(%f,%f,%f)\n", posCam.x, posCam.y, posCam.z);

}
////////////////////////////////////////////////////////////////////////////
void computeMatricesFromInputs2(GLFWwindow* hWindow){
	static int inited = 0;
	if(inited == 0) {//Jammy fixed bug
	    glfwSetInputMode(hWindow,GLFW_CURSOR,GLFW_CURSOR_NORMAL );
	    glfwSetCursorPos(hWindow, 1024/2, 768/2);
	    inited = 1;
	}


    switch(m_eCurrentMode) {
        case VM_BIRDVIEW:
            doBirdView(hWindow); break;
        case VM_BEHIND:
        	doBehindView(hWindow); break;
        case VM_AHEAD:
        	doAheadView(hWindow); break;
        case VM_BACK:
        	doBackView(hWindow); break;
        case VM_FRONT:
        	doFrontView(hWindow); break;
        case VM_LEFT:
        	doLeftView(hWindow); break;
        case VM_RIGHT:
        	doRightView(hWindow); break;
        case VM_SAT:
        	doSatView(hWindow); break;
    }


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(m_FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
		posCam,           // Camera is here
		posLookAt,//
		camUp//up                  // Head is up (set to 0,-1,0 to look upside-down)
	);


    static float fTz = 0;
    static float fRy = 0;

    ObjectMatrix = mat4(0);
	if (glfwGetKey( hWindow, GLFW_KEY_A ) == GLFW_PRESS){
		fRy -= CAR_TURN_STEP;
	}
	else if (glfwGetKey( hWindow, GLFW_KEY_D ) == GLFW_PRESS){
        fRy += CAR_TURN_STEP;
	}
    if (glfwGetKey( hWindow, GLFW_KEY_W ) == GLFW_PRESS){
            //car forward
        fTz -= CAR_MOVE_STEP;
        if (fTz < -CAR_MAX_MOVE)fTz = -CAR_MAX_MOVE;
    }
    else if (glfwGetKey( hWindow, GLFW_KEY_X ) == GLFW_PRESS){
        fTz += CAR_MOVE_STEP;
        if (fTz > CAR_MAX_MOVE) fTz = CAR_MAX_MOVE;
    }
    if (glfwGetKey( hWindow, GLFW_KEY_S ) == GLFW_PRESS){
        fTz = 0; fRy = 0;
    }
    //printf("z=%f\n", fTz);

    mat4 tx = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,fTz,1);
float cy=  cos(fRy);
float sy=  sin(fRy);
    ObjectMatrix = mat4(
    					cy, 0, sy, 0,
                        0,  1, 0, 0,
                        -sy,0,cy, 0,
                        0,0,0,1)*tx;

	// For the next frame, the "last time" will be "now"
	//lastTime = currentTime;
}
//typedef void (* GLFWkeyfun)(GLFWwindow*,int,int,int,int);

/*! @brief The function signature for Unicode character callbacks.
 *
 *  This is the function signature for Unicode character callback functions.
 *
 *  @param[in] window The window that received the event.
 *  @param[in] codepoint The Unicode code point of the character.
 *
 *  @sa glfwSetCharCallback
 *
 *  @ingroup input
 */
void onKeyEvent(GLFWwindow* hWindow, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Y && action == GLFW_PRESS){
			g_bShowCar = !g_bShowCar;
	//check view mode
	} else if (glfwGetKey( hWindow, GLFW_KEY_K ) == GLFW_PRESS){
		m_eCurrentMode = VM_BIRDVIEW;
	} else if(glfwGetKey( hWindow, GLFW_KEY_P ) == GLFW_PRESS) {
		m_eCurrentMode = VM_BEHIND;
	} else if(glfwGetKey( hWindow, GLFW_KEY_O ) == GLFW_PRESS) {
		m_eCurrentMode = VM_AHEAD;
	} else if(glfwGetKey( hWindow, GLFW_KEY_I ) == GLFW_PRESS) {
		m_eCurrentMode = VM_FRONT;
	} else if(glfwGetKey( hWindow, GLFW_KEY_M ) == GLFW_PRESS) {
		m_eCurrentMode = VM_BACK;
	} else if(glfwGetKey( hWindow, GLFW_KEY_L ) == GLFW_PRESS) {
		m_eCurrentMode = VM_LEFT;
	} else if(glfwGetKey( hWindow, GLFW_KEY_J ) == GLFW_PRESS) {
		m_eCurrentMode = VM_RIGHT;
	} else if(glfwGetKey( hWindow, GLFW_KEY_T ) == GLFW_PRESS) {
		m_eCurrentMode = VM_SAT;
	}
}
