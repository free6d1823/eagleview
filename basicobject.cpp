#include <vector>
#include <iostream>

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


#include "basicobject.h"

BasicObject::BasicObject()
{
    m_isInited = false;
    m_vao = 0;
    m_vboVertex = 0;
    m_vboIndex = 0;
    m_programID = 0;
    m_texture = 0;
    m_textureID = 0;
    m_uvBuffer = 0; 
    m_normalBuffer = 0;

    m_lightID = 0;
}
BasicObject::~BasicObject()
{
}

void BasicObject::init()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    m_programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
    m_mvpMatrixID = glGetUniformLocation(m_programID, "MVP");
    m_viewMatrixID = glGetUniformLocation(m_programID, "V");
    m_modelMatrixID = glGetUniformLocation(m_programID, "M");

    m_texture = loadDDS("uvmap.DDS");
    m_textureID = glGetUniformLocation(m_programID, "myTextureSampler");
    //
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("kid.obj", vertices, uvs, normals);

    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

    glGenBuffers(1, &m_vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_vboIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    m_numToDraw = indices.size();

	// Get a handle for our "LightPosition" uniform
    glUseProgram(m_programID);
    m_lightID = glGetUniformLocation(m_programID, "LightPosition_worldspace");

    m_modelMatrix = glm::mat4(0.01,0,0,0, 0,0,0.01,0, 0,0.01,0,0,  0,-1,0,1);
    m_isInited = true;
    m_modelMatrixNew = m_modelMatrix; //keep the original state
}

void BasicObject::transform(mat4& transform)
{
    m_modelMatrixNew = m_modelMatrix*transform;
}
void BasicObject::update(glm::mat4& pojection, glm::mat4& view, glm::vec3& light)
{
    glm::mat4 MVP = pojection * view * m_modelMatrixNew;
    // Use our shader
    glUseProgram(m_programID);

    glUniform3f(m_lightID, light.x, light.y, light.z);
    glUniformMatrix4fv(m_viewMatrixID, 1, GL_FALSE, &view[0][0]); 

    glUniformMatrix4fv(m_mvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(m_modelMatrixID, 1, GL_FALSE, &m_modelMatrix[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(m_textureID, 0);


}

void BasicObject::cleanup()
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
    if (m_normalBuffer) {
        glDeleteBuffers(1, &m_normalBuffer);
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

void BasicObject::draw()
{
    if (!m_isInited) {
        std::cout << "please call init() before draw()" << std::endl;
    }
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

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
    glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);

    // Draw the triangles !
    glDrawElements(
			GL_TRIANGLES,      // mode
			m_numToDraw,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

