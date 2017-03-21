// myNewGl.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"

#include "Camera.h"
#include "Shaders.h"
#include "TexturedModel.h"
#include "modelLoading.h"
#include "SkyBox.h"
#include "Spiral.h"

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
	const std::array<std::string, 6> faces3 = {
		"skybox/graycloud_ft.jpg",
		"skybox/graycloud_bk.jpg",

		"skybox/graycloud_up.jpg",
		"skybox/graycloud_dn.jpg",


		"skybox/graycloud_rt.jpg",
		"skybox/graycloud_lf.jpg",
	};

	const std::array<std::string, 6> faces4 = {
		"skybox/6.png",
		"skybox/1.png",
		"skybox/5.png",
		"skybox/4.png",
		"skybox/3.png",
		"skybox/2.png",
	};


	return SkyBox(faces3);
}


struct SizePoint {
	glm::vec3 pos;
	glm::vec3 color;
	float size;
};


struct PointProgram {
	PointProgram(GLuint program, const std::vector<SizePoint>& points) :program(program)
	{
		MVPplcation = glGetUniformLocation(program, "MVP");
		size = points.size();
		vertexArray = createVertexArray();
		vertexBuffer = createArrayBuffer(points);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

		glBindVertexArray(0);
	}

	void draw(const glm::mat4& mvp) {
		glUseProgram(program);
		glUniformMatrix4fv(MVPplcation, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(vertexArray);
		glDrawArrays(type, 0, size);
		glBindVertexArray(0);
	}

	GLsizei size = 0;
	GLenum type = GL_POINTS;
	GLuint vertexArray;
	GLuint vertexBuffer;

	GLuint MVPplcation;
	GLuint program;
};

typedef MultipleTransformationWraper<PointProgram> Cone;

Cone createCone() {
	glm::vec3 color1 = { 0,1,0 };
	glm::vec3 color2 = { 1,0,0 };

	std::vector<PointProgram> programs;
	{
		int i = 0;
		std::vector<SizePoint> points;
		points.push_back({ {0,0,0},{0,0,0},0 });
	
		for (float angle = 0; angle < PI * 2; angle += PI * 2 / 8)
		{
			points.push_back({ {sin(angle),cos(angle),0},i % 2 == 0 ? color1 : color2 , 0.f });
			i++;
		}
		points.push_back({ { sin(0),cos(0),0 },{ 0,0,0 },0 });
		programs.emplace_back(Shaders::getInstance().getProgram(Shaders::vertex::FLAT_POINT, Shaders::fragment::FLAT_COLOR), points);
		programs.back().type = GL_TRIANGLE_FAN;

	}

	{
		int i = 0;
		std::vector<SizePoint> points;
		points.push_back({ { 0,0,1 },{ 0,0,0 },0 });

		for (float angle = 0; angle < PI * 2; angle += PI * 2 / 8)
		{
			points.push_back({ { sin(angle),cos(angle),0 },i % 2 == 0 ? color1 : color2 , 0.f });
			i++;
		}
		points.push_back({ { sin(0),cos(0),0 },{ 0,0,0 },0 });
		programs.emplace_back(Shaders::getInstance().getProgram(Shaders::vertex::FLAT_POINT, Shaders::fragment::FLAT_COLOR), points);
		programs.back().type = GL_TRIANGLE_FAN;
	}

	return Cone(programs);

}


class Scene {
public:
	Scene(GLFWwindow * window):camera(window),cameraControll(window)
	{
	//	t.transform = glm::translate(glm::mat4(1.f), glm::vec3(1, 1, 1));
		//pyramid.transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -3));
		pyramid2.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0, -3.5f, -3)));
	//	spiral.translate = glm::translate(glm::mat4(1.f), glm::vec3(-3, -5, -3));
		cone.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(3, 0, -3)));
		spiral.setTranslation(glm::translate(glm::mat4(), {-3,-5,-3}));
	}

	void updata(float time) {
		cameraControll.updataCamera(time, camera);
		camera.updateAspectRation();

		skyBox.draw(camera);

		spiral.draw(camera);
		spiral.update(time);
		cone.draw(camera);


		pyramid.draw(camera);
		pyramid2.draw(camera);

		auto pos = camera.getCameraPos();




	}
private:
	Sprite sprite = createSprite("awesomeface.png");

	Model pyramid = loadModel("pyramid.dae", "pyramid.png");
	Model pyramid2{ loadModel("pir.dae","pir.png") };
	Spiral spiral = createSpiral();
	SkyBox skyBox = createSkyBox();
	Cone cone = createCone();

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

