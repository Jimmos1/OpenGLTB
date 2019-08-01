#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdio.h>

#include <glad/glad.h>  // Initialize with gladLoadGL
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <imgui/imfilebrowser.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void imguiWindow(GLFWwindow *window, bool* show_demo_window, bool* show_another_window, ImVec4* clear_color, ImVec4* materialAmbient, ImVec4* materialDiffuse, ImVec4* materialSpecular, string* str);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ImGUI variables
bool show_demo_window = false;
bool show_another_window = false;
ImVec4 custom_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
bool toMoveCam = false;
ImGui::FileBrowser fileDialog;
float objectShininess = 0.5f;
float lightIntensityMultiplier = 1.0f;
float light0AmountConstant = 0.5f;
float light0AmountLinear = 0.5f;
float light0AmountQuadratic = 0.005f;
float light1AmountConstant = 0.5f;
float light1AmountLinear = 0.5f;
float light1AmountQuadratic = 0.005f;

ImVec4 materialAmbient { 0.2f, 0.2f, 0.2f, 0.0f};
ImVec4 materialDiffuse { 0.5f, 0.5f, 0.5f, 0.0f };
ImVec4 materialSpecular { 1.0f, 1.0f, 1.0f, 0.0f };
// lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	const char* glsl_version = "#version 130";
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
	glfwSwapInterval(1); // Enable VSync
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse and how
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialize OpenGL loader
	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile shaders
	Shader ourShader("model_loading.vs", "model_loading.fs");

	// load models
	std::string str = "nanosuit/nanosuit.obj";
	Model ourModel(str.c_str());
	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(-5.0f, 0.0f, 0.0f)
	};

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		// Per-frame time logic
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		//std::cout << ourModel.directory + "|" + str.substr(0, str.find_last_of('/')) << std::endl;
		if (ourModel.directory != str.substr(0, str.find_last_of('/')))
		{
			ourModel.Reload(str);
		}

		// Render
		glClearColor(0.0f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		ourShader.use();
		//ourShader.setVec3("spotLight.position", camera.Position);
		//ourShader.setVec3("spotLight.direction", camera.Front);
		//ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setFloat("material.shininess", objectShininess);
		ourShader.setVec3("material.ambient", glm::vec3(materialAmbient.x + materialAmbient.y + materialAmbient.z));
		ourShader.setVec3("material.diffuse", glm::vec3(materialDiffuse.x + materialDiffuse.y + materialDiffuse.z));
		ourShader.setVec3("material.specular", glm::vec3(materialSpecular.x + materialSpecular.y + materialSpecular.z));

		ourShader.setFloat("lightIntensity", lightIntensityMultiplier);
		// light properties
		//ourShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
		//ourShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
		//ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		//ourShader.setFloat("spotLight.constant", 1.0f);
		//ourShader.setFloat("spotLight.linear",0.2f);
		//ourShader.setFloat("spotLight.quadratic", 0.032f);

		// point light 1
		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("pointLights[0].diffuse", 0.1f, 1.0f, 0.1f);
		ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[0].constant", light0AmountConstant);
		ourShader.setFloat("pointLights[0].linear", light0AmountLinear);
		ourShader.setFloat("pointLights[0].quadratic", light0AmountQuadratic);

		// point light 2
		ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		ourShader.setVec3("pointLights[1].ambient", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("pointLights[1].diffuse", 1.0f, 0.0f, 0.0f);
		ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[1].constant", light1AmountConstant);
		ourShader.setFloat("pointLights[1].linear", light1AmountLinear);
		ourShader.setFloat("pointLights[1].quadratic", light1AmountQuadratic);


		// View/ projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		// Render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, (float)glm::radians((float)glfwGetTime() * 10), glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourShader.setVec3("cameraPos", camera.Position);

		ourModel.Draw(ourShader);

		//ImGUI code
		imguiWindow(window, &show_demo_window, &show_another_window, &custom_color, &materialAmbient, &materialDiffuse, &materialSpecular, &str);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void imguiWindow(
	GLFWwindow* window,
	bool* show_demo_window,
	bool* show_another_window,
	ImVec4* custom_color,
	ImVec4* materialAmbient,
	ImVec4* materialDiffuse,
	ImVec4* materialSpecular,
	string* str)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (*show_demo_window)
	{
		ImGui::ShowDemoWindow(show_demo_window);
	}


	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		//static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Editor");                          // Create a window called "Hello, world!" and append into it.
		{
			ImGui::Text("This will be some useful text.");               // Display some text (you can use a format strings too)
			//ImGui::Checkbox("Demo Window", show_demo_window);      // Edit bools storing our window open/close state
			//ImGui::Checkbox("Another Window", show_another_window);

			if (ImGui::Button("Open file dialog (NOT WORK)"))
				fileDialog.Open();

			fileDialog.Display();

			if (fileDialog.HasSelected())
			{
				std::cout << "Selected filename: " << fileDialog.GetSelected().string() << std::endl;
				*str = fileDialog.GetSelected().string();
				std::cout << &str << std::endl;
				cin >> *str;

				fileDialog.ClearSelected();
			}
			ImGui::Text("Material Properties");
			ImGui::SliderFloat("Object Shininess", &objectShininess, 0.0f, 5.0f);
			ImGui::SliderFloat("Light Intensity Multiplier", &lightIntensityMultiplier, 0.0f, 5.0f);
			ImGui::ColorEdit3("Material Ambient", (float*)materialAmbient); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Material Diffuse", (float*)materialDiffuse); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Material Specular", (float*)materialSpecular); // Edit 3 floats representing a color

			ImGui::BeginGroup();
			ImGui::Text("Light 0 Properties");
			ImGui::SliderFloat("Constant ", &light0AmountConstant, 0.0f, 5.0f);
			ImGui::SliderFloat("Linear ", &light0AmountLinear, 0.0f, 5.0f);
			ImGui::SliderFloat("Quadratic ", &light0AmountQuadratic, 0.0f, 5.0f);
			ImGui::EndGroup();

			ImGui::Text("Light 1 Properties");
			ImGui::SliderFloat("Constant", &light1AmountConstant, 0.0f, 5.0f);
			ImGui::SliderFloat("Linear", &light1AmountLinear, 0.0f, 5.0f);
			ImGui::SliderFloat("Quadratic", &light1AmountQuadratic, 0.0f, 5.0f);
																	 
			//if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			//{
			//	counter++;
			//}
			//ImGui::Text("counter = %d", counter);
			//ImGui::SameLine(); // Adds the previous function to the next one
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}

	// 3. Show another simple window.
	if (*show_another_window)
	{
		ImGui::Begin("Another Window", show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		{
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				*show_another_window = false;
		}
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(SHIFT_DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.ProcessKeyboard(SHIFT_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(REL_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(REL_DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	if (toMoveCam)
	{
		float xoffset = (float)xpos - lastX;
		float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

		lastX = (float)xpos;
		lastY = (float)ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever you click buttons on mouse, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		toMoveCam = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true; // Override so that mouse doesn't jump to new position on update

	}
	else
	{
		toMoveCam = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (toMoveCam)
	{
		camera.ProcessMouseScroll((float)yoffset);
	}
}