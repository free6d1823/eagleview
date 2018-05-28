#include <vector>
#include <iostream>
#include <stdio.h>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

//use SOIL to load texture
#include "SOIL/SOIL.h"

#include "sphere.h"
#include "calibration.h"


#define GET_REMAP_V(v, k, i, N) {		\
	float x = PI_2*(1 -(float)i/N);	\
	float a1 = x + atan(k* tan(x)); \
	v = 1-a1/PI;}

bool Sphere::loadSphere(float r, std::vector < glm::vec3 > & vertices, std::vector < glm::vec2 > & uvs)
{
    int i,j;
    const float dv = 1.0/lats;
    const float du = 1.0/longs;
    float kk = (2.0-SPHERE_REMAP_RATIO)/SPHERE_REMAP_RATIO; //ratio of small_r/big_r= 0.5
    float u0 = 0;
    float v1,v2;
    float x0,y0,z0; 
    float x1,y1,z1;
    float x2,z2; //y2=y0
    float x3,z3; //y3=y1
    double theda, phi=PI_2;    
    const double dtheda = -2*PI/longs; //reversed map
    const double dphi = PI/lats;
 
    v2=dv;
    GET_REMAP_V(v2, kk, (float)0,(float)lats);

    for(i = 1; i < lats; i++) {
        u0 = 0;
        v1 = v2;
        //v2 += dv;
        GET_REMAP_V(v2, kk, (float)i,(float)lats);
        phi -= dphi;
        theda = 2*PI; //reverse map 0;
        y0 = r*sin(phi);
        y1 = r*sin(phi-dphi);
        x0 = 0; 
        z0 = r*cos(phi);

        double phi2 = phi - dphi;
//    p0 <--p2 (phi)
//     |   ^/^
//     |  // |
//     | //  |
//     V/V   |
//    p1 -->p3 (phi2)
        for(j = 0; j <= longs; j++) {
            double theda2 = theda + dtheda;
            if(theda2 <0 )theda2 -= 2*PI;
            //horizontal reverse map
            x1 = r*(float) (cos(phi2)*sin(theda));
            x2 = r*(float) (cos(phi)*sin(theda2));
            x3 = r*(float) (cos(phi2)*sin(theda2));
            z1 = r*(float) (cos(phi2)*cos(theda));        
            z2 = r*(float) (cos(phi)*cos(theda2));
            z3 = r*(float) (cos(phi2)*cos(theda2));
            vertices.push_back(glm::vec3(x0,y0,z0));
            vertices.push_back(glm::vec3(x2,y0,z2));
            vertices.push_back(glm::vec3(x1,y1,z1));
            vertices.push_back(glm::vec3(x1,y1,z1));
            vertices.push_back(glm::vec3(x2,y0,z2));
            vertices.push_back(glm::vec3(x3,y1,z3));
//update x0,z0, theda
            theda = theda2;
            x0 = x2; z0 = z2; 

            uvs.push_back(glm::vec2(u0, v1));
            uvs.push_back(glm::vec2(u0+du, v1));
            uvs.push_back(glm::vec2(u0, v2));
            uvs.push_back(glm::vec2(u0, v2));
            uvs.push_back(glm::vec2(u0+du, v1));
            uvs.push_back(glm::vec2(u0+du, v2));
            u0 += du;
 
       }
   }
 

}

Sphere::Sphere()
{
    m_isInited = false;
    m_vao = 0;
    m_vboVertex = 0;
    m_vboIndex = 0;
//private data
m_vertexPositionID = 0;
    lats = 64;
    longs = 64;
    m_programID = 0;
}

Sphere::~Sphere()
{

}
void Sphere::init()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    m_programID = LoadShaders("sphereShader.vert", "sphereShader.frag");
    //get SL uniform
    m_mvpMatrixID = glGetUniformLocation(m_programID, "MVP");

    m_texture = SOIL_load_OGL_texture("sphere.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO| SOIL_FLAG_MIPMAPS);

    if (m_texture> 0 ) {
	printf("Load texture OK\n");
    } else {
	printf("Load texture failed: %s\n", SOIL_last_result() );
        m_texture = loadDDS("img_test.dds");
    }
    m_textureID = glGetUniformLocation(m_programID, "myTextureSampler");

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //update to vertices
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    bool res = loadSphere(SPHERE_RADIUS, vertices, uvs);

    glGenBuffers(1, &m_vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    m_numToDraw = uvs.size();
    //move sphere above ground
    m_modelMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, SPHERE_RADIUS-0.01, 0.0f));//sink floor by 1, so that not block car
    glm:mat4 rot = glm::mat4(cos(START_LONGITUDE), 0.0, sin(START_LONGITUDE), 0.0,
 				0.0,1.0,0,0.0,
 				-sin(START_LONGITUDE), 0, cos(START_LONGITUDE),0,
 				0,0,0,1);
    m_modelMatrix = m_modelMatrix* rot;

    m_isInited = true;
}
void Sphere::update(glm::mat4& pojection, glm::mat4& view, glm::vec3& light)
{
    glm::mat4 MVP = pojection * view * m_modelMatrix;

    glUseProgram(m_programID);
    glUniformMatrix4fv(m_mvpMatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(m_textureID, 0);
}
void Sphere::cleanup()
{
    if (!m_isInited) {
        return;
    }
    if(m_vboVertex) {
        glDeleteBuffers(1, &m_vboVertex);
    }
    if(m_vboIndex) {
        glDeleteBuffers(1, &m_vboIndex);
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_uvBuffer) {
        glDeleteBuffers(1, &m_uvBuffer);
    }
    if(m_programID)
	glDeleteProgram(m_programID);
    if(m_texture)
	glDeleteTextures(1, &m_texture);

    m_isInited = false;
    m_vao = 0;
    m_vboVertex = 0;
    m_vboIndex = 0;
}

void Sphere::draw()
{
    if (!m_isInited) {
        std::cout << "please call init() before draw()" << std::endl;
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
   // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
    glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
   // Index buffer
//jammy    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
     

    glDrawArrays(GL_TRIANGLES, 0, m_numToDraw); 

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

