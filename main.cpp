// RobotElements class and constructors and animation functions written by Parker Drake
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "MatrixStack.h"
#include "Program.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

char* vertShaderPath = "../shaders/shader.vert";
char* fragShaderPath = "../shaders/shader.frag";

GLFWwindow* window;
glm::vec3 eye(0.0f, 0.0f, 20.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

// Index for tree branches
int index = 0;

// Angles for camera rotation
float theta = 3.14 / 2;
float phi = 3.14 / 2;

// Mouse position trackers
float lastx, lasty;

// Animate flag
bool animate = false;

Program program;
MatrixStack modelViewProjectionMatrix;

// Draw cube on screen
void DrawCube(glm::mat4& modelViewProjectionMatrix)
{
	program.SendUniformData(modelViewProjectionMatrix, "mvp");
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

class RobotElements
{
private:

public:
	RobotElements* parent = NULL; // Pointer to limb's parent
	std::vector<RobotElements*> children; // Pointer to limb's children
	glm::vec3 transRelParent = glm::vec3(0, 0, 0); // Current translation from parent limb
	glm::vec3 rotRelJoint = glm::vec3(0, 0, 0); // Current rotation relative to parent joint {X, Y, Z}
	glm::vec3 transRelJoint = glm::vec3(0, 0, 0); // Current translation relative to parent joint
	glm::vec3 scaleFactor = glm::vec3(0, 0, 0); // Current scale of limb

	RobotElements(RobotElements* p, glm::vec3 trp, glm::vec3 rrj, glm::vec3 trj, glm::vec3 sf)
	{
		parent = p;
		transRelParent = trp;
		rotRelJoint = rrj;
		transRelJoint = trj;
		scaleFactor = sf;
	}
	~RobotElements() {}


	void DrawLimb() // Recursively draw each component by calling DrawLimb to draw children
	{
		modelViewProjectionMatrix.pushMatrix(); // Copy the parent (top) matrix

		// Perform transformations based on stored, current transform
		modelViewProjectionMatrix.translate(transRelJoint); // Translate away from joint

		modelViewProjectionMatrix.translate(transRelParent); // Translate away from parent


		modelViewProjectionMatrix.rotateX(rotRelJoint[0]); // Rotate on the x-axis
		modelViewProjectionMatrix.rotateY(rotRelJoint[1]); // Rotate on the y-axis
		modelViewProjectionMatrix.rotateZ(rotRelJoint[2]); // Rotate on the z-axis

		modelViewProjectionMatrix.translate(glm::vec3(transRelJoint[0] * -1, transRelJoint[1] * -1, transRelJoint[2] * -1)); // Translate towards joint

		modelViewProjectionMatrix.pushMatrix(); // Push before scaling to scale locally, draw before popping
		modelViewProjectionMatrix.scale(scaleFactor); // Scale by scaleFactor

		DrawCube(modelViewProjectionMatrix.topMatrix()); // Draw the current limb

		modelViewProjectionMatrix.popMatrix();



		for (int i = 0; i < children.size(); i++) // Call DrawLimb() on each child recursively
		{
			children[i]->DrawLimb();
		}


		modelViewProjectionMatrix.popMatrix(); // Pop current limb from the stack - done with this limb
	}

};

// Pointers to torso and current selected limb
RobotElements* torsoPtr;
RobotElements* limbPtr;

void ConstructRobot()
{
	RobotElements* torso = new RobotElements(NULL, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.1, 2.2, 0.88));
	RobotElements* head = new RobotElements(torso, glm::vec3(0, 2.5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(.5, 0.5, 0.5));

	RobotElements* upperLeftArm = new RobotElements(torso, glm::vec3(2, 1.5, 0), glm::vec3(0, 0, 0), glm::vec3(-1.5, 0, 0), glm::vec3(1, 0.4, 0.4));
	RobotElements* lowerLeftArm = new RobotElements(upperLeftArm, glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(-1.5, 0, 0), glm::vec3(1, 0.3, 0.3));

	RobotElements* upperRightArm = new RobotElements(torso, glm::vec3(-2, 1.5, 0), glm::vec3(0, 0, 0), glm::vec3(1.5, 0, 0), glm::vec3(1, 0.4, 0.4));
	RobotElements* lowerRightArm = new RobotElements(upperRightArm, glm::vec3(-2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.5, 0, 0), glm::vec3(1, 0.3, 0.3));

	RobotElements* upperLeftLeg = new RobotElements(torso, glm::vec3(0.5, -4, 0), glm::vec3(0, 0, 0), glm::vec3(0, 2.5, 0), glm::vec3(0.45, 2, 0.5));
	RobotElements* lowerLeftLeg = new RobotElements(upperLeftLeg, glm::vec3(0, -4, 0), glm::vec3(0, 0, 0), glm::vec3(0, 2.5, 0), glm::vec3(0.35, 2, 0.4));

	RobotElements* upperRightLeg = new RobotElements(torso, glm::vec3(-0.5, -4, 0), glm::vec3(0, 0, 0), glm::vec3(0, 2.5, 0), glm::vec3(0.45, 2, 0.5));
	RobotElements* lowerRightLeg = new RobotElements(upperRightLeg, glm::vec3(0, -4, 0), glm::vec3(0, 0, 0), glm::vec3(0, 2.5, 0), glm::vec3(0.35, 2, .4));

	// Push all limbs to their respective children vectors
	torso->children.push_back(head);

	torso->children.push_back(upperLeftArm);
	upperLeftArm->children.push_back(lowerLeftArm);

	torso->children.push_back(upperRightArm);
	upperRightArm->children.push_back(lowerRightArm);

	torso->children.push_back(upperLeftLeg);
	upperLeftLeg->children.push_back(lowerLeftLeg);

	torso->children.push_back(upperRightLeg);
	upperRightLeg->children.push_back(lowerRightLeg);

	// Set pointers to the torso
	torsoPtr = torso;
	limbPtr = torso;
}


void Display()
{
	program.Bind();

	modelViewProjectionMatrix.loadIdentity();
	modelViewProjectionMatrix.pushMatrix();

	// Setting the view and Projection matrices
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	modelViewProjectionMatrix.Perspective(glm::radians(60.0f), float(width) / float(height), 0.1f, 100.0f);

	// Setting the position of the camera
	

	modelViewProjectionMatrix.LookAt(eye, center, up);

	// Drawing the robot
	torsoPtr->DrawLimb();
	modelViewProjectionMatrix.popMatrix();

	program.Unbind();

}

void runningAnimation()
{
	torsoPtr->rotRelJoint = glm::vec3(0.8, 0, 0); // Torso position
	torsoPtr->children[0]->rotRelJoint = glm::vec3(-0.5, 0, 0); // Head position
	torsoPtr->children[1]->rotRelJoint = glm::vec3(0.0, 1, 0); // Left upper arm position
	torsoPtr->children[1]->children[0]->rotRelJoint = glm::vec3(0.0, 0, 0); // Left lower arm position
	torsoPtr->children[2]->rotRelJoint = glm::vec3(0.0, -1, 0); // Right upper arm position
	torsoPtr->children[2]->children[0]->rotRelJoint = glm::vec3(0.0, 0, 0); // Right lower arm position
	torsoPtr->children[3]->rotRelJoint = glm::vec3(-2, 0, 0); // Left upper leg position
	torsoPtr->children[3]->children[0]->rotRelJoint = glm::vec3(2, 0, 0); // Left lower leg position
	torsoPtr->children[4]->rotRelJoint = glm::vec3(0.0, 0, 0); // Right upper leg position
	torsoPtr->children[4]->children[0]->rotRelJoint = glm::vec3(0.0, 0, 0); // Right lower leg position

	while (animate)
	{
		double  time = glfwGetTime();
		double frequency = 6;

		torsoPtr->transRelParent = glm::vec3(0, 0.75 * sin(2 * frequency * time - 3.14 / 10), 0); // Torso bounce
		torsoPtr->rotRelJoint = glm::vec3(0.8, 0.1 * sin(frequency * time), 0); // Torso twist

		torsoPtr->children[1]->rotRelJoint = glm::vec3(0, 1, 0.2 * sin(frequency * time) - 0.5);
		torsoPtr->children[2]->rotRelJoint = glm::vec3(0, -1, 0.2 * sin(frequency * time) + 0.5);


		torsoPtr->children[3]->rotRelJoint = glm::vec3(sin(frequency * time) - 1, 0, 0);
		torsoPtr->children[3]->children[0]->rotRelJoint = glm::vec3((-1 * sin(frequency * time) + 1), 0, 0);

		torsoPtr->children[4]->rotRelJoint = glm::vec3((-1 * sin(frequency * time) - 1), 0, 0);
		torsoPtr->children[4]->children[0]->rotRelJoint = glm::vec3((sin(frequency * time) + 1), 0, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && GLFW_PRESS == action)
		std::cout << "Mouse left button is pressed." << std::endl;
}

void ScrollCallback(GLFWwindow* lWindow, double xoffset, double yoffset)
{
	glm::vec3 b = eye - center;
	if (yoffset == 1)
	{
		b /= 1.1;
	}
	if (yoffset == -1)
	{
		b *= 1.1;
	}
	eye = b + center;
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int state2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		theta = -0.02 * (xpos - lastx);
		phi = -0.02 * (ypos - lasty);

		glm::vec3 b = (eye - center);
		glm::vec4 b4(b[0], b[1], b[2], 1);
		glm::vec4 up4(up[0], up[1], up[2], 1);

		glm::vec3 right = glm::normalize(glm::cross(up, b));
				
		glm::mat4 thetaMat = glm::rotate(glm::mat4(1.0f), theta, up);
		glm::mat4 phiMat = glm::rotate(glm::mat4(1.0f), phi, right);

		b4 = phiMat * thetaMat * b4;
		up4 = phiMat * thetaMat * up4;

		b = glm::vec3(b4[0], b4[1], b4[2]);
		up = glm::vec3(up4[0], up4[1], up4[2]);

		eye = center + b;
	}
	if (state2 == GLFW_PRESS)
	{
		eye[0] += 0.02 * (xpos - lastx);
		center[0] += 0.02 * (xpos - lastx);

		eye[1] -= 0.02 * (ypos - lasty);
		center[1] -= 0.02 * (ypos - lasty);
	}	

	lastx = xpos;
	lasty = ypos;
}


// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key)
	{
	case ',':
		// Next Limb
		limbPtr->scaleFactor /= 1.1;
		if (limbPtr != torsoPtr)
		{
			if (limbPtr->parent != torsoPtr)
			{
				limbPtr = limbPtr->parent;
			}
			else if (limbPtr->parent == torsoPtr)
			{
				if (index == 0)
				{
					limbPtr = torsoPtr;
				}
				else
				{
					index--;
					limbPtr = torsoPtr->children[index];
					if (limbPtr->children.size() > 0)
					{
						limbPtr = limbPtr->children.back();
					}
				}
			}
		}		
		limbPtr->scaleFactor *= 1.1;
		break;
	case '.':
		// Previous limb
		limbPtr->scaleFactor /= 1.1;
		if (limbPtr == torsoPtr)
		{
			limbPtr = limbPtr->children[index];
		}
		else if (limbPtr->children.size() > 0)
		{
			limbPtr = limbPtr->children[0];
		}
		else if (limbPtr->children.size() == 0)
		{
			if (index < torsoPtr->children.size() - 1)
			{
				index++;
				limbPtr = torsoPtr->children[index];
			}			
		}
		limbPtr->scaleFactor *= 1.1;
		break;
	case 'x':
		limbPtr->rotRelJoint -= glm::vec3(0.1, 0, 0);
		break;
	case 'X':
		limbPtr->rotRelJoint += glm::vec3(0.1, 0, 0);
		break;
	case 'y':
		limbPtr->rotRelJoint -= glm::vec3(0, 0.1, 0);
		break;
	case 'Y':
		limbPtr->rotRelJoint += glm::vec3(0, 0.1, 0);
		break;
	case 'z':
		limbPtr->rotRelJoint -= glm::vec3(0, 0, 0.1);
		break;
	case 'Z':
		limbPtr->rotRelJoint += glm::vec3(0, 0, 0.1);
		break;
	case '~':
		if (!animate)
		{
			animate = true;
			runningAnimation();
		}
		else
			animate = false;
		break;
	}
}

void CreateCube()
{
	// x, y, z, r, g, b, ...
	float cubeVerts[] = {
		// Face x-
		-1.0f,	+1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		// Face x+
		+1.0f,	+1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		// Face y-
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		// Face y+
		+1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		// Face z-
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		// Face z+
		+1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f
	};

	GLuint vertBufferID;
	glGenBuffers(1, &vertBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	GLint posID = glGetAttribLocation(program.GetPID(), "position");
	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	GLint colID = glGetAttribLocation(program.GetPID(), "color");
	glEnableVertexAttribArray(colID);
	glVertexAttribPointer(colID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

}

void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Init()
{
	glfwInit();
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Realtime Animation", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	program.SetShadersFileName(vertShaderPath, fragShaderPath);
	program.Init();

	ConstructRobot();
	CreateCube();
}


int main()
{
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}