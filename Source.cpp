#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

Camera camera1(glm::vec3(0.0f, 0.0f, 5.0f));
Camera camera2(glm::vec3(-1.0f, 0.0f, 5.0f));
Camera camera3(glm::vec3(1.0f, 0.0f, 5.0f));
Camera* camera = &camera1;

enum ShaderType
{
	BASIC_LIGHTING,
	FOG,
	PHONG,
	GOURAUD,
	FLAT
};

ShaderType currentShader = BASIC_LIGHTING;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	Model pawnModel("pawn/12931_WoodenChessPawnSideA_v1_l3.obj"); // this shoul not be flipped
	stbi_set_flip_vertically_on_load(true);

	// build and compile shader programs
	Shader BasicLightningShader("1.model_loading.vs", "1.model_loading.fs");
	Shader PhongShader("2.2.basic_lighting.vs", "2.2.basic_lighting.fs");
	Shader FogShader("2.2.basic_lighting.vs", "shader_fog.fs");
	Shader GouraudShader("gouraud_shader.vs", "gouraud_shader.fs");
	Shader FlatShader("flat_shader.vs", "flat_shader.fs");

	Shader* modelShader = &BasicLightningShader;

	// load models
	Model backpackModel("backpack/backpack.obj");
	Model containerModel("Container/12281_Container_v2_L2.obj");

	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// initialize matrices
		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		// Setup light
		//glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 7.0f);

		// Upadting cameras:
		float radius = 1.5f;
		float backpack_x = static_cast<float>(sin(glfwGetTime()) * radius);
		float backpack_z = static_cast<float>(cos(glfwGetTime()) * radius);
		float backpack_y = static_cast<float>(cos(glfwGetTime()) * radius);

		// changing camera
		glm::vec3 cameraTarget = glm::vec3(backpack_x, backpack_y, backpack_z);
		glm::vec3 cameraDirection = glm::normalize(camera2.Position - cameraTarget);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

		if (camera == &camera2)
			view = glm::lookAt(camera->Position, cameraTarget, cameraUp);
		else
			view = camera->GetViewMatrix();

		camera3.Position = cameraTarget + glm::vec3(0, 0, 4.0f);
		camera3.updateCameraVectors();

		// Choosing Shader:
		switch (currentShader)
		{
		case BASIC_LIGHTING:
			modelShader = &BasicLightningShader;
			break;
		case FOG:
			modelShader = &FogShader;
			break;
		case PHONG:
			modelShader = &PhongShader;
			break;
		case GOURAUD:
			modelShader = &GouraudShader;
			break;
		case FLAT:
			modelShader = &FlatShader;
			break;
		default:
			modelShader = &BasicLightningShader;
			break;
		}
		modelShader->use();
		modelShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		modelShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		modelShader->setVec3("lightPos", lightPos);
		modelShader->setVec3("viewPos", camera->Position);

		// Drawing backpack:
		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//view = camera->GetViewMatrix();
		modelShader->setMat4("projection", projection);
		modelShader->setMat4("view", view);
		model = glm::mat4(1.0f);
		// lets make it move around the scene:
		model = glm::translate(model, glm::vec3(backpack_x, backpack_y, backpack_z));
		// Dodaj obrót wokó³ w³asnej osi Y
		float rotationAngle = glfwGetTime(); // Mo¿esz dostosowaæ ten k¹t w zale¿noœci od potrzeb
		model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelShader->setMat4("model", model);
		backpackModel.Draw(*modelShader);

		// Drawing chess pawn:
		//modelShader->use();
		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		modelShader->setMat4("projection", projection);
		modelShader->setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f)); // translate at the center of the scene
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelShader->setMat4("model", model);
		pawnModel.Draw(*modelShader);

		// draw container:
		glm::mat4 projectionModel = glm::mat4(1.0f);
		glm::mat4 viewModel = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		projectionModel = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		viewModel = view;
		modelShader->setMat4("projection", projectionModel);
		modelShader->setMat4("view", viewModel);
		glm::mat4 modelModel = glm::mat4(1.0f);
		modelModel = glm::translate(modelModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		modelModel = glm::rotate(modelModel, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelModel = glm::rotate(modelModel, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelModel = glm::scale(modelModel, glm::vec3(0.003f, 0.003f, 0.003f));	// it's wayy too big for our scene, so scale it down
		modelShader->setMat4("model", modelModel);
		containerModel.Draw(*modelShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		camera = &camera1;
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		camera = &camera2;
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		camera = &camera3;
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		currentShader = PHONG;
	}
	else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		currentShader = FOG;
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		currentShader = BASIC_LIGHTING;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		currentShader = GOURAUD;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		currentShader = FLAT;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}