#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <string>
#include <glad/glad.h>// za rad sa OpenGL f-jama
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <Kamera.h>
#include <Shader.h>
#include <Model.h>
//#include <IrrKlang/irrKlang.h>//za audio


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void init_sline();
//parametre scale1,scale2 i color1,color2 sam postavio kako bi kocke i obodi kocki bili
//nezavisni
void lightCube(Shader& lightCubeShader, Shader& stencilShader, unsigned int lightCubeVAO, 
	float scale1,float scale2,glm::vec3 color1,glm::vec3 color2,
	glm::mat4 model, glm::mat4 view, glm::mat4 projection, double current_time);

//irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();


glm::vec3 bezier(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
unsigned int loadTexture(const char* path);

//podesavamo velicinu prozora
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//pozivamo konstruktor za kameru
Camera camera(glm::vec3(0.0, 0.0, 3.0));
float lastX = (float)SCR_WIDTH / 2.0f; 
float lastY = (float)SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

std::vector<glm::vec3> slinePosition;
std::vector<glm::vec3> slineViewDirection;
unsigned int slineIndex;

int main() {
	glfwInit();
	//minor verzija konteksta je 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//major verzija konteksta je 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//koristimo openGL core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Moj Projekat", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create a window";
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
//	SoundEngine->play2D("DreamItPossible.mp3", true);
//	SoundEngine->setSoundVolume(0.1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD";
		glfwTerminate();
		return EXIT_FAILURE;
	}
	//necemo menjati operator poredjenja za dubinu
	//tj koristicu GL_LESS(podrazumevani) kako bismo imali realan efekat
	//da objekti koji su blizi near ravni zaklanjaju dalje objekte(koji imaju vecu z vr)
	//ukoliko im se delovi(fragmenti) projektuju na istu poziciju prostora ekrana
	//Ne smemo zaboraviti da u petlji renderovanja postavimo da se cisti i depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Shader instancingShader("instancing.frag", "instancing.vert");
	init_sline();

	Shader ourShader("resources/shaders/plane.fs", "resources/shaders/plane.vs");
	Shader houseShader("resources/shaders/house.fs", "resources/shaders/house.vs");
	Model ourModel("resources/objects/Cartoon_house_low_poly_OBJ.obj");
	Shader lampShader("resources/shaders/lamp.fs", "resources/shaders/lamp.vs");
	Model lampModel("resources/objects/rv_lamp post 1.obj");

	Shader lightingShader("resources/shaders/lighting.fs", "resources/shaders/lighting.vs");
	Shader lightCubeShader("resources/shaders/lightCube.fs", "resources/shaders/lightCube.vs");
	Shader stencilShader("resources/shaders/stencil.fs", "resources/shaders/stencil.vs");
	glm::mat4 perspectiveMatrix;
	GLuint uniformMatrixLoaction;
	
		// PLANE

	float planeVertices[] = {
	       //pozicije   //koordinate tekstura   
		5.0f, -0.5f,  5.0f,  1.0f, 0.0f,	//desno gore
		5.0f, -0.5f, -5.0f,  1.0f, 1.0f,	//desno dole
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,	//levo gore
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,	//levo dole
	};
	
	unsigned int planeIndices[] = {
		0,1,2,
		3,2,1
	};

	//instancing cube
	float instancing_cubeVertices[] = {
		0.1f, 0.1f, 0.1f, 0.0f, 0.9f, 0.9f,
	   -0.1f, 0.1f, 0.1f, 0.0f, 0.9f, 0.9f,
	    0.1f,-0.1f, 0.1f, 0.0f, 0.9f, 0.9f,
	   -0.1f,-0.1f, 0.1f, 0.0f, 0.9f, 0.9f,

	    0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 1.0f,
		0.1f,-0.1f, 0.1f, 0.1f, 0.1f, 1.0f,
		0.1f, 0.1f,-0.1f, 0.1f, 0.1f, 1.0f,
		0.1f,-0.1f,-0.1f, 0.1f, 0.1f, 1.0f,

		0.1f, 0.1f,  0.1f,	0.1f, 0.1f, 0.1f,  
		0.1f, 0.1f, -0.1f,	0.1f, 0.1f, 0.1f,  
		-0.1f,0.1f,  0.1f,	0.1f, 0.1f, 0.1f,  
		-0.1f,0.1f, -0.1f,	0.1f, 0.1f, 0.1f,  

		 0.1f,  0.1f, -0.1f, 1.0f, 0.1f, 0.1f,  
		 0.1f, -0.1f, -0.1f, 1.0f, 0.1f, 0.1f,  
		-0.1f,  0.1f, -0.1f, 1.0f, 0.1f, 0.1f,  
		-0.1f, -0.1f, -0.1f, 1.0f, 0.1f, 0.1f,  

		-0.1f,  0.1f,  0.1f, 0.1f, 1.0f, 0.1f,  
		-0.1f,  0.1f, -0.1f, 0.1f, 1.0f, 0.1f,  
		-0.1f, -0.1f,  0.1f, 0.1f, 1.0f, 0.1f,  
		-0.1f, -0.1f, -0.1f, 0.1f, 1.0f, 0.1f,  

		0.1f, -0.1f, 0.1f,	0.7f, 0.7f, 0.7f, 
	   -0.1f, -0.1f, 0.1f,	0.7f, 0.7f, 0.7f, 
		0.1f, -0.1f,-0.1f,	0.7f, 0.7f, 0.7f, 
	   -0.1f, -0.1f,-0.1f,	0.7f, 0.7f, 0.7f  
	};

	unsigned int instancing_cubeIndices[] = {
		0,1,2,
		2,1,3,

		4,5,6,
		6,5,7,

		8,9,10,
		10,9,11,

		12,13,14,
		14,13,15,

		16,17,18,
		18,17,19,

		20,21,22,
		22,21,23
	};

	unsigned int counter = 0;
	std::vector<float> offsets;
	const int start = -65 / 2;
	const int end = 65 / 2;
	unsigned int numberOfOffsets = 0;
	unsigned int numberOfOffsetsToDraw = 0;
	for (int x = start; x <= end; x++)
	{
		for (int y = start; y <= end; y++)
		{
			for (int z = start; z <= end; z++)
			{
				offsets.emplace_back((float)x * 5.0f);
				offsets.emplace_back((float)y * 5.0f);
				offsets.emplace_back((float)z * 5.0f);
				numberOfOffsetsToDraw++;
				numberOfOffsets += 3;
				counter++;
			}
		}
	}

	float CubeVertices[] = {
		//prednja strana	      //normale	    //koordinate teksture
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		//zadnja strana
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		//leva strana
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		//desna strana
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 //donja strana
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		//gornja strana
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
	};
	//pozicije kutija koje predstavljaju hangar
	glm::vec3 cubePositions[] = {
		glm::vec3(-5.0f,  -0.45f,    5.0f),
		glm::vec3(-4.5f,  -0.45f,    5.0f),
		glm::vec3(-4.0f,  -0.45f,    5.0f),
		glm::vec3(-3.5f,  -0.45f,    5.0f),
		glm::vec3(-3.0f,  -0.45f,    5.0f),
		glm::vec3(-2.5f,  -0.45f,    5.0f),
		glm::vec3(-5.0f,   0.05f,    5.0f),
		glm::vec3(-4.5f,   0.05f,    5.0f),
		glm::vec3(-4.0f,   0.05f,    5.0f),
		glm::vec3(-3.5f,   0.05f,    5.0f),
		glm::vec3(-3.0f,   0.05f,    5.0f),
		glm::vec3(-5.0f,   0.55f,    5.0f),
		glm::vec3(-4.5f,   0.55f,    5.0f),
		glm::vec3(-4.0f,   0.55f,    5.0f),
		glm::vec3(-3.5f,   0.55f,    5.0f),
		glm::vec3(-5.0f,   1.05f,    5.0f),
		glm::vec3(-4.5f,   1.05f,    5.0f),
		glm::vec3(-5.0f,   1.05f,    5.0f)
	};
	//instancing cube Vertices
	unsigned int icubeVAO,icubeVBO;
	glGenVertexArrays(1, &icubeVAO);
	glBindVertexArray(icubeVAO);

	glGenBuffers(1, &icubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER,icubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(instancing_cubeVertices), instancing_cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//offsets
	unsigned int offsetVBO;
	glGenBuffers(1, &offsetVBO);
	glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
	glBufferData(GL_ARRAY_BUFFER, numberOfOffsets*sizeof(float), &offsets[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1);

	//instancing_cube_indices
	unsigned int instancingCubeIndicesEBO;
	glGenBuffers(1, &instancingCubeIndicesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instancingCubeIndicesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(instancing_cubeIndices), instancing_cubeIndices, GL_STATIC_DRAW);


	//cube
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//light cube
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//plane 
	unsigned int planeVAO, planeVBO,planeEBO;

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &planeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glBindVertexArray(0);


	unsigned int planeTexture = loadTexture("grass.jpg");
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	ourShader.use();
	ourShader.setUniform1i("plane", 0);

	lightingShader.use();
	lightingShader.setUniform1i("material.diffuse", 0);
	lightingShader.setUniform1i("material.specular", 1); 
	while (!glfwWindowShouldClose(window)) {

		float current_time = glfwGetTime();
		deltaTime = current_time - lastFrame;
		lastFrame = current_time;
		
		process_input(window);
		//cistimo bafer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		//instancingcubes
		glBindVertexArray(icubeVAO);
		instancingShader.use();
		instancingShader.setMat4("view", view);
		instancingShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		instancingShader.setMat4("model", model);
		glDrawElementsInstanced(GL_TRIANGLES,sizeof(instancing_cubeIndices),GL_UNSIGNED_INT,0,numberOfOffsetsToDraw);
		//plane
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		ourShader.use();
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.5, 1.5, 1.5));
		ourShader.setMat4("model", model);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
		

		//HOUSE
		lightPos.x = 0.5 * sin(current_time) - 2.0f;
		lightPos.y = sin(current_time) * 0.3 + 2.9f;
		lightPos.z = 0.5 * cos(current_time) - 6.0f;
		
		float scale1 = 0.2f;
		float scale2 = 0.25;
		glm::vec3 color = glm::vec3((float)cos(current_time) * 0.5 + 0.5f, (float)sin(current_time) * 0.5f + 0.5, 0.5f);
		glm::vec3 color1 = glm::vec3(0.0f, 0.0f, 1.0f);
		lightCube(lightCubeShader, stencilShader, lightCubeVAO, scale1, scale2, color, color1, model, view, projection, current_time);
		houseShader.use();
		
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		model = glm::translate(model, glm::vec3(1050.0f, -155.0f, -1050));
		houseShader.setMat4("model", model);
		houseShader.setMat4("view", view);
		houseShader.setMat4("projection", projection);
		
		houseShader.setVec3("viewPos", camera.Position);
		houseShader.setVec3("light.position",lightPos);
		houseShader.setVec3("light.ambient", glm::vec3(1.0));
		houseShader.setVec3("light.diffuse", glm::vec3(1.0f));
		houseShader.setVec3("light.specular", glm::vec3(1.0f));

		houseShader.setVec3("material.ambient", glm::vec3(0.005));
		houseShader.setVec3("material.diffuse", glm::vec3((float)cos(current_time) * 0.5 + 0.5f, (float)sin(current_time) * 0.5f + 0.5, 0.5f));
		houseShader.setVec3("material.specular", glm::vec3(0.3));
		houseShader.setUniform1f("material.shininess", 32.0f);

		ourModel.Draw(houseShader);
		
		//lamp object
		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::translate(model, glm::vec3(-10.0f, -3.7f, -30));
		lampShader.setMat4("model", model);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		
		lampModel.Draw(lampShader);
		
		//KOCKA

		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		//svojstva svetla
		
		lightPos.y =  sin(glfwGetTime())*sin(glfwGetTime());
		lightPos.x = sin(glfwGetTime()) - 4.5f;
		lightPos.z = cos(glfwGetTime()) + 5.0f;
		
		lightingShader.setVec3("directionalLight.direction", -0.2f,-1.0f,-0.3f);
		lightingShader.setVec3("directionalLight.ambient", 0.05f,0.05f,0.05f);
		lightingShader.setVec3("directionalLight.diffuse", 0.4f,0.4,0.4f);
		lightingShader.setVec3("directionalLight.specular", 0.5f,0.5f,0.5f);
		lightingShader.setVec3("pointLight.position", lightPos);
		lightingShader.setVec3("pointLight.ambient",glm::vec3(0.5f));
		lightingShader.setVec3("pointLight.diffuse", glm::vec3(1.0f));
		lightingShader.setVec3("pointLight.specular", glm::vec3(1.0f));
		lightingShader.setUniform1f("pointLight.constant", 1.0f);
		lightingShader.setUniform1f("pointLight.linear", 0.09f);
		lightingShader.setUniform1f("pointLight.quadratic", 0.032f);
		//svojstva materijala
		lightingShader.setVec3("material.diffuse", 0.0f, 0.50980392f, 0.50980392f);
		lightingShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
		lightingShader.setUniform1f("material.shininess", 32.0f);

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		
		glBindVertexArray(cubeVAO);
		unsigned int num_of_elements = sizeof(cubePositions)/sizeof(cubePositions[0]);
		for (unsigned int i = 0; i <num_of_elements ; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		scale1 = 0.2f;
		scale2 = 0.25f;
		color = glm::vec3(sin(glfwGetTime()), cos(glfwGetTime()), 0.5f);
		color1 = glm::vec3(1.0f, 0.0f, 0.0f);
		lightCube(lightCubeShader, stencilShader, lightCubeVAO, scale1, scale2, color, color1, model, view, projection, current_time);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/*glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);*/
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glfwTerminate();
	return 0;
}

void lightCube(Shader& lightCubeShader,Shader& stencilShader,unsigned int lightCubeVAO,float scale1,float scale2,glm::vec3 color1,glm::vec3 color2,
	glm::mat4 model,glm::mat4 view,glm::mat4 projection,double current_time) {
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glBindVertexArray(lightCubeVAO);
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		lightCubeShader.setVec3("color", color1);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(scale1));
		lightCubeShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		stencilShader.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(scale2));
		stencilShader.setMat4("view", view);
		stencilShader.setMat4("projection", projection);
		stencilShader.setMat4("model", model);
		stencilShader.setVec3("color", color2);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//		SoundEngine->setSoundVolume(SoundEngine->getSoundVolume() + 0.2f);
//	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//		SoundEngine->setSoundVolume(SoundEngine->getSoundVolume() - 0.2f);
	
}
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
//kada god pomerimo mis 
//ova funkcija ce biti pozvana
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void init_sline() {
	std::vector<glm::vec3> slinePositionPoints;
	slinePositionPoints.emplace_back(glm::vec3(0.0f,0.0f,10.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 10.0f, 7.5f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 5.0f, 0.0f));
	slinePositionPoints.emplace_back(glm::vec3(5.0f, 3.75f, -5.0f));
	slinePositionPoints.emplace_back(glm::vec3(10.0f, 2.5f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(5.0f, 1.25f, 5.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(0.0f, 25.0f, 0.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 150.0f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(-5.0f, 145.0f, 0.0f));
	slinePositionPoints.emplace_back(glm::vec3(-10.0f, 140.0f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(0.0f, 100.0f, 50.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 75.0f, 25.0f));

	slinePositionPoints.emplace_back(glm::vec3(0.0f, 35.5f, 12.5f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, -5.0f, 0.0f));

	slinePositionPoints.emplace_back(glm::vec3(0.0f, -2.5f, -5.0f));
	slinePositionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));

	/*SLINE VIEW DIRECTION*/
	std::vector<glm::vec3> slineViewDirectionPoints;
	slineViewDirectionPoints.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.5f, 0.0f, -0.5f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.7f, 0.0f, -0.3f));

	slineViewDirectionPoints.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.5f, 0.0f, 0.5f));
	slineViewDirectionPoints.emplace_back(glm::vec3(-1.0f, 0.0f, 0.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.8f, -0.2f, 0.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.0f, -0.8f, 0.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.01f, -1.0f, 0.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(-0.01f, -1.0f, 0.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.01f, -1.0f, 0.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(-0.01f, -1.0f, 0.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.25f, -0.25f, -0.25f));
	slineViewDirectionPoints.emplace_back(glm::vec3(-0.5f, -0.5f, -0.5f));

	slineViewDirectionPoints.emplace_back(glm::vec3(-0.25f, -0.25f, -0.75f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.0f, 0.0f, -1.0f));

	slineViewDirectionPoints.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
	slineViewDirectionPoints.emplace_back(glm::vec3(0.0f, 0.0f, 1.0f));

	int index = 0;
	while (index < slinePositionPoints.size() - 2)
	{
		for (float t = 0.0f; t < 1.0f; t += 0.002f)
		{
			slinePosition.emplace_back(bezier(t, slinePositionPoints.at(index), slinePositionPoints.at(index + 1), slinePositionPoints.at(index + 2)));
			slineViewDirection.emplace_back(bezier(t, slineViewDirectionPoints.at(index), slineViewDirectionPoints.at(index + 1), slineViewDirectionPoints.at(index + 2)));
		}
		index += 2;
	}

	slineIndex = 0;
}

glm::vec3 bezier(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
	glm::vec3 result;
	result.x = (powf(1 - t, 2.0f) * p0.x) + (2 * t * (1 - t) * p1.x) + (powf(t,2.0f)*p2.x);
	result.y = (powf(1 - t, 2.0f) * p0.y) + (2 * t * (1 - t) * p1.y) + (powf(t, 2.0f) * p2.y);
	result.z = (powf(1 - t, 2.0f) * p0.z) + (2 * t * (1 - t) * p1.z) + (powf(t, 2.0f) * p2.z);
	return result;
}

