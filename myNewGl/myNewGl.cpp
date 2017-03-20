// myNewGl.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"

#include "Camera.h"
#include "Shaders.h"
#include "TexturedModel.h"
#include "modelLoading.h"
#include "SkyBox.h"


#define PI 3.14159265359f
struct Point3d {
	glm::vec3 pos;
	glm::vec3 color;
	GLfloat size;
};


struct ProgramPoint {
	ProgramPoint(GLuint program):program(program)
	{
		MVPplcation = glGetUniformLocation(program, "MVP");
		totalSizeLocation = glGetUniformLocation(program,"totalSize");
		spiralPositionLocation = glGetUniformLocation(program, "spiralPosition");

	}

	void draw(int size) {
	}
	void record(const std::vector<Point3d>& points) {
		size = points.size();

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point3d), points.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

	}
	void draw(const glm::mat4& mvp,GLfloat totalSize = 0.f,GLfloat spiralPosition = 0.f) {
		glUseProgram(program);
		glUniformMatrix4fv(MVPplcation, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1f(spiralPositionLocation, spiralPosition);
		glUniform1f(totalSizeLocation, totalSize);

		glBindVertexArray(vertexArray);
		glDrawArrays(type, 0, size);
		glBindVertexArray(0);


	}

	GLsizei size = 0;
	GLenum type = GL_POINTS;

	GLuint vertexArray;
	GLuint vertexBuffer;

	GLuint MVPplcation;
	GLuint totalSizeLocation;
	GLuint spiralPositionLocation;
	GLuint program;

};

template<int vertex,int fragment>
struct Points {
	Points(const std::vector<Point3d>& points)
	{
		program.record(points);
	}
	void draw(const Camera& camera) {


		glm::mat4 mvp = camera.projection()*camera.view()*translate;
		program.draw(mvp,totalSize,spiralPosition);

	}
	void update(float time) {
		spiralPosition += time*spiralVel;
		if (spiralPosition > 1.f) {
			spiralPosition = spiralPosition - 1.f;
		}
	}
	glm::mat4 translate;
	float spiralVel = 0.02;
	float spiralPosition = 0;
	float totalSize = 0;

	ProgramPoint program = Shaders::getInstance().getProgram<ProgramPoint>(vertex, fragment);
private:

	GLuint vertexBuffer;
	GLuint vertexArray;
};
typedef Points<Shaders::vertex::SPIRAL, Shaders::fragment::IN_OUT_COLOR> Spiral;
typedef Points<Shaders::vertex::FLAT_POINT, Shaders::fragment::FLAT_COLOR> FlatColorPoints;

Spiral createSpiral() {
	std::vector<Point3d> points = {};

	float y = 0;
	for (float i = 0; i < PI * 2 * 11; i += 0.1f) {

		points.push_back({ { sin(i),y,cos(i) },{ 0.0,1.0,0.0 },10.f });
		y += 0.01f;
	}
	Spiral p(points);
	p.totalSize = y;
	return p;
};



SkyBox createSkyBox() {
	const std::array<std::string, 6> faces = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/back.jpg",
		"skybox/front.jpg"
	};
	const std::array<std::string, 6> faces2 = {
		"skybox/Skybox_PositiveX.jpg",
		"skybox/Skybox_NegativeX.jpg",
		"skybox/Skybox_PositiveY.jpg",
		"skybox/Skybox_NegativeY.jpg",
		"skybox/Skybox_PositiveZ.jpg",
		"skybox/Skybox_NegativeZ.jpg",
	};

	return SkyBox(faces);
}



template<typename T>
struct DrawColection {
	std::vector<T> elements;

	void draw(const Camera& camera) {
		for (auto& element : elements) {
			element.draw(camera);
		}
	}
	void transform(glm::mat4 transform) {
		for (auto& element : elements) {
			element.translate = transform;
		}

	}

};

DrawColection<FlatColorPoints> createCone() {
	glm::vec3 color1 = { 0,1,0 };
	glm::vec3 color2 = { 1,0,0 };

	DrawColection<FlatColorPoints> collection;


	{
		int i = 0;
		std::vector<Point3d> points;
		points.push_back({ {0,0,0},{0,0,0},0 });
	
		for (float angle = 0; angle < PI * 2; angle += PI * 2 / 8)
		{
			points.push_back({ {sin(angle),cos(angle),0},i % 2 == 0 ? color1 : color2 , 0.f });
			i++;
		}
		points.push_back({ { sin(0),cos(0),0 },{ 0,0,0 },0 });
		FlatColorPoints p(points);
		p.program.type = GL_TRIANGLE_FAN;
		collection.elements.emplace_back(p);
	}

	{
		int i = 0;
		std::vector<Point3d> points;
		points.push_back({ { 0,0,1 },{ 0,0,0 },0 });

		for (float angle = 0; angle < PI * 2; angle += PI * 2 / 8)
		{
			points.push_back({ { sin(angle),cos(angle),0 },i % 2 == 0 ? color1 : color2 , 0.f });
			i++;
		}
		points.push_back({ { sin(0),cos(0),0 },{ 0,0,0 },0 });
		FlatColorPoints p(points);
		p.program.type = GL_TRIANGLE_FAN;
		collection.elements.emplace_back(p);
	}




	return collection;
}


class Scene {
public:
	Scene(GLFWwindow * window):camera(window),cameraControll(window)
	{
		t.transform = glm::translate(glm::mat4(1.f), glm::vec3(1, 1, 1));
		pyramid.transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -3));
		pyramid2.transform = glm::translate(glm::mat4(1.f), glm::vec3(0, -3.5f, -3));
		spiral.translate = glm::translate(glm::mat4(1.f), glm::vec3(-3, -5, -3));
		cone.transform(glm::translate(glm::mat4(1.f), glm::vec3(3, 0, -3))*glm::rotate(glm::mat4(), glm::radians(-90.f), {1,0,0}));
	}

	void updata(float time) {
		cameraControll.updataCamera(time, camera);
		camera.updateAspectRation();

		skyBox.draw(camera);
		pyramid.draw(camera);
		pyramid2.draw(camera);
		cone.draw(camera);
	
		spiral.draw(camera);
		spiral.update(time);

		auto pos = camera.getCameraPos();




	}
private:
	TextureModel t{createSprite("awesomeface.png")};
	TextureModel pyramid{ loadModel("pyramid.dae","pyramid.png") };
	TextureModel pyramid2{ loadModel("pir.dae","pir.png") };

	SkyBox skyBox{ createSkyBox() };
	Spiral spiral{createSpiral()};
	DrawColection<FlatColorPoints> cone = createCone();

	Camera camera;
	CameraControll cameraControll;
	
};




class Renderer {
public:

	Renderer(GLFWwindow* window):window(window),scene(window)
	{


		

	}
	void loop() {
		while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0)
		{
			static auto prevTime = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			float time = static_cast<std::chrono::duration<float>> (now - prevTime).count();
			prevTime = std::chrono::high_resolution_clock::now();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene.updata(time);


			glfwSwapBuffers(window);
			glfwPollEvents();

		}
	}

private:
	GLFWwindow* window;
	Scene scene;

};

int main()
{

	if (!glfwInit()) {
		throw std::runtime_error("failed to initialize glfw");
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "OPENGL", NULL, NULL);

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Faliled to initialize glew");
	}
	glEnable(GL_PROGRAM_POINT_SIZE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	Renderer r(window);
	r.loop();

    return 0;
}

