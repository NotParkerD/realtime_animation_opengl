// Matrix Stack transformation functions written by Parker Drake
#include "MatrixStack.h"

#include <stdio.h>
#include <cassert>
#include <vector>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

MatrixStack::MatrixStack()
{
	mstack = make_shared< stack<glm::mat4> >();
	mstack->push(glm::mat4(1.0));
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::pushMatrix()
{
	const glm::mat4 &top = mstack->top();
	mstack->push(top);
	assert(mstack->size() < 100);
}

void MatrixStack::popMatrix()
{
	assert(!mstack->empty());
	mstack->pop();
	// There should always be one matrix left.
	assert(!mstack->empty());
}

void MatrixStack::loadIdentity()
{
	glm::mat4 &top = mstack->top();
	top = glm::mat4(1.0);
}

void MatrixStack::translate(const glm::vec3 &t)
{
	glm::mat4 translationMatrix(1.0f);

	// Need to comment out the following line and write your own version
	//translationMatrix = glm::translate(glm::mat4(1.0f), t);

	float A[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		t[0], t[1], t[2], 1
	};
	translationMatrix = glm::make_mat4(A);
	multMatrix(translationMatrix);
}

void MatrixStack::scale(const glm::vec3& s)
{
	glm::mat4 scaleMatrix(1.0);

	// Need to comment out the following line and write your own version
	//scaleMatrix = glm::scale(glm::mat4(1.0f), s);

	float A[16] =
	{
		s[0], 0, 0, 0,
		0, s[1], 0, 0,
		0, 0, s[2], 0,
		0, 0, 0, 1
	};

	scaleMatrix = glm::make_mat4(A);
	multMatrix(scaleMatrix);
}

void MatrixStack::rotateX(float angle)
{
	glm::mat4 rotationMatrix(1.0f);

	// Need to comment out the following line and write your own version
	//rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

	float A[16] =
	{
		1, 0, 0, 0,
		0, cos(angle), sin(angle), 0,
		0, -1 * sin(angle), cos(angle), 0,
		0, 0, 0, 1
	};

	rotationMatrix = glm::make_mat4(A);

	multMatrix(rotationMatrix);
}

void MatrixStack::rotateY(float angle)
{
	glm::mat4 rotationMatrix(1.0f);

	// Need to comment out the following line and write your own version
	//rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

	float A[16] =
	{
		cos(angle), 0, -1 * sin(angle), 0,
		0, 1, 0, 0,
		sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1
	};

	rotationMatrix = glm::make_mat4(A);

	multMatrix(rotationMatrix);
}

void MatrixStack::rotateZ(float angle)
{
	glm::mat4 rotationMatrix(1.0f);

	// Need to comment out the following line and write your own version
	//rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

	float A[16] =
	{
		cos(angle), sin(angle), 0, 0,
		-1 * sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	rotationMatrix = glm::make_mat4(A);

	multMatrix(rotationMatrix);
}

void MatrixStack::multMatrix(glm::mat4 &matrix)
{
	glm::mat4 &top = mstack->top();

	// Need to comment out the following line and write your own version
	//top *= matrix;

	float A[16] =
	{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};

	glm::mat4 temp = glm::make_mat4(A);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				temp[i][j] += matrix[i][k] * top[k][j];
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			top[i][j] = temp[i][j];
		}
	}



	// Getting a pointer to the glm::mat4 matrix
	float* topArray = glm::value_ptr(top);
	float* matrixArray = glm::value_ptr(matrix);
}

void MatrixStack::Perspective(float fovy, float aspect, float near, float far)
{
	glm::mat4 projectionMatrix(0.0f);

	// Need to comment out the following line and write your own version
	//projectionMatrix = glm::perspective(fovy, aspect, near, far);
	float d = 1 / tan(fovy / 2);
	float A[16] =
	{
		d/aspect, 0, 0, 0,
		0, d, 0, 0,
		0, 0, -1 * ((far + near)/(far - near)), -1,
		0, 0, -2 * far * near / (far - near), 0
	};
	projectionMatrix = glm::make_mat4(A);
	multMatrix(projectionMatrix);
}

void MatrixStack::LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	glm::mat4 viewMatrix(1.0f);

	// Need to comment out the following line and write your own version
	//viewMatrix = glm::lookAt(eye, center, up);
	glm::vec3 w(glm::normalize(eye - center));
	glm::vec3 u(glm::normalize(glm::cross(up, w)));
	glm::vec3 v(glm::cross(w, u));

	float A[16] =
	{
		u[0], v[0], w[0], 0,
		u[1], v[1], w[1], 0,
		u[2], v[2], w[2], 0,
		-(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]), -(v[0] * eye[0] + v[1] * eye[1] + v[2] * eye[2]), -(w[0] * eye[0] + w[1] * eye[1] + w[2] * eye[2]), 1
	};

	viewMatrix = glm::make_mat4(A);


	multMatrix(viewMatrix);
}


void MatrixStack::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}

void MatrixStack::scale(float x, float y, float z)
{
	scale(glm::vec3(x, y, z));
}

void MatrixStack::scale(float s)
{
	scale(glm::vec3(s, s, s));
}

glm::mat4 &MatrixStack::topMatrix()
{
	return mstack->top();
}

void MatrixStack::print(const glm::mat4 &mat, const char *name)
{
	if(name) {
		printf("%s = [\n", name);
	}
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			// mat[j] returns the jth column
			printf("%- 5.2f ", mat[j][i]);
		}
		printf("\n");
	}
	if(name) {
		printf("];");
	}
	printf("\n");
}

void MatrixStack::print(const char *name) const
{
	print(mstack->top(), name);
}
