#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#define PI  3.1415926535897932384626433832795f
#define PI_2 (PI/2)

/* sphere background adjust
 *
 */
/* the longitude start angle, in gradiun, adjust to front in world coordinate */
#define START_LONGITUDE		(150*PI/180)
/* the ration of small sphere radius to big sphere radius, (0, 1] */
#define SPHERE_REMAP_RATIO	0.25
/* radius of the sphere */
#define SPHERE_RADIUS		20

/* 3D car model adjust
 *
 */
#define CAR_HEIGHT	2.1
#define CAR_SCALE	2.1
#define CAR_ORIENTATION	(0*PI/180)
/* car movement */
#define CAR_MOVE_STEP	0.05
#define CAR_TURN_STEP	0.01
#define CAR_MAX_MOVE	2.5
/*BirdView Mode setting */
#define BIRDVIEW_MAX_ZOOM	SPHERE_RADIUS*2
#define BIRDVIEW_MIN_ZOOM	CAR_HEIGHT*4
#define BIRDVIEW_STEP_ZOOM	1
#define BIRDVIEW_ROTATE_STEP	0.02
/*VM_BEHIND Mode setting */
#define BEHIND_FOV			120
#define BEHIND_MAX_ZOOM			7
#define BEHIND_MIN_ZOOM			0.4
#define BEHIND_STEP_ZOOM		0.2
#define BEHIND_ROTATE_STEP		0.02
/*VM_AHEAD Mode setting */
#define AHEAD_FOV			120
#define AHEAD_MAX_ZOOM			7
#define AHEAD_MIN_ZOOM			0.4
#define AHEAD_STEP_ZOOM		0.2
#define AHEAD_ROTATE_STEP		0.02
/* VM_BACK settings */
#define BACK_FOV	90
#define BACK_MAX_ZOOM			SPHERE_RADIUS*2
#define BACK_MIN_ZOOM			0
#define BACK_STEP_ZOOM		1
#define BACK_ROTATE_STEP		0.02

/* VM_FRONT settings */
#define FRONT_FOV	120
#define FRONT_MAX_ZOOM			SPHERE_RADIUS*2
#define FRONT_MIN_ZOOM			0
#define FRONT_STEP_ZOOM		1
#define AHEAD_ROTATE_STEP		0.02


#endif //CALIBRATION_H_
