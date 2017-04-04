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
#include "Colider.h"
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include "Stearing.h"
#include "Colider.h"


struct position {
	glm::vec3 pos;
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

using namespace entityx;

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
	class Instance {
	public:
		Instance(GLsizei size,GLuint vertexArray,GLuint MVPlocation,GLuint program):size(size)
		{

		}
		GLenum type = GL_POINTS;

	private:
		GLsizei size;
		GLuint vertexArray;
		GLuint MVPplcation;
		GLuint program;

	};


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


GLFWwindow* Input::_window;


struct DrawingSystem : public System<DrawingSystem> {

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {

		ComponentHandle<Camera> cameraHandle;
		Entity cameraEntity = *es.entities_with_components(cameraHandle).begin();
		ComponentHandle<Camera> camera = cameraEntity.component<Camera>();
		es.each<Transformation, TextureModelPrototype::Instance>(
			[dt,&camera](Entity entity, Transformation &t, TextureModelPrototype::Instance &instance){

			const auto mvp = camera->projection*camera->view*t.createModel();
			instance.draw(mvp);

		});
	};
};

struct SpiralSystem : public System<SpiralSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {
		ComponentHandle<Camera> cameraHandle;
		Entity cameraEntity = *es.entities_with_components(cameraHandle).begin();
		ComponentHandle<Camera> camera = cameraEntity.component<Camera>();

		es.each<Transformation,SpiralPrototype::SpiralInstance, SpiralPosition>(
			[dt,&camera](Entity entity, Transformation &t, SpiralPrototype::SpiralInstance instance, SpiralPosition &position) {

			position.spiralposition += dt*position.spiralVel;
			if (position.spiralposition > 1.f) {
				position.spiralposition = position.spiralposition - 1.f;
			}

			const auto mvp = camera->projection*camera->view*t.createModel();
			instance.draw(mvp, position.spiralSize,position.spiralposition);

		});
	};

};



class Scene : public EntityX {
public:
	Scene(GLFWwindow * window):window(window)
	{
		systems.add<DrawingSystem>();
		systems.add<CameraSystem>();
		systems.add<InputSystem>(window);
		systems.add <SpiralSystem>();
		systems.configure();
		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->rotation = glm::quat({0.5,0.5,0});
			entity.assign<TextureModelPrototype::Instance>(singlePiramidProto.createInstance());
			entity.assign<aabbColider*>(&singlePiramidProto.colider);
		}

		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			//entity.component<Transformation>()->rotation = { 0.5,0.5,0 };
			entity.component<Transformation>()->position= { 3,-5,-6 };
			entity.assign<TextureModelPrototype::Instance>(domekProto.createInstance());
			entity.assign<aabbColider*>(&domekProto.colider);
		}
		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 3,-5,-6 };
			entity.assign<SpiralPrototype::SpiralInstance>(spiralProto.createInstance());
			entity.assign<SpiralPosition>()->spiralSize;
			entity.assign<aabbColider*>(&spiralProto.colider);

			entity.component<SpiralPosition>()->spiralSize = spiralProto.totalSize;
		}

		Entity camera = entities.create();
		camera.assign<Position>();
		camera.assign<CameraDirection>();
		camera.assign<Camera>();
		this->camera = camera.component<Camera>();

		pyramid.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0, 0.f, -3)));
		//pyramid2.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(0, -3.f, -3)));

	//	spiral.translate = glm::translate(glm::mat4(1.f), glm::vec3(-3, -5, -3));
		cone.setTranslation(glm::translate(glm::mat4(1.f), glm::vec3(3, 0, -3)));

		//domek.setTranslation(glm::translate(glm::mat4(), { 12,0,-1 }));
		//domek.setRotation(glm::rotate(glm::mat4(), glm::radians(90.f), { 1,0,0 }));
		//neptun.setTranslation(glm::translate(glm::mat4(), { 8,0,-3 }));
	//	neptun.setRotation(glm::rotate(glm::mat4(),glm::radians(90.f), { 1,0,0 }));
		//Input::mouseDownCallbacks.push_back(std::bind(&Scene::mouseDown,this));

	}

	//void mouseDown() {
	//	if (currentlyMainpulatedObject != nullptr) {
	//		currentlyMainpulatedObject = nullptr;
	//		return;
	//	}

	//	float distance;
	//	if (singlePiramidProto.colider.testRay(cameraControll.position(), cameraControll.front(), m.getModel(), distance)) {
	//		currentlyMainpulatedObject = &m;
	//		std::cout << distance << std::endl;
	//	}
	//}



	TransformationWraper * currentlyMainpulatedObject = nullptr;


	void updata(float time) {
		//cameraControll.updataCamera(time, camera);
	//	camera.updateAspectRation();
		skyBox.draw(camera);
		systems.update<InputSystem>(time);
		systems.update<CameraSystem>(time);
		systems.update<DrawingSystem>(time);
		systems.update<SpiralSystem>(time);


		//cone.draw(camera);
		//neptun.draw(camera);

		//spiral.draw(camera);
		//spiral.update(time);

		//m.draw(camera);

		//pyramid.draw(camera);
		//pyramid2.draw(camera);


		//domek.draw(camera);

		//stearing.update(time);
		//stearing.draw(camera);


//		if (currentlyMainpulatedObject!= nullptr) {
//			auto& pos = cameraControll.position();
//			auto& front = cameraControll.front();
//			currentlyMainpulatedObject->setTranslation(glm::translate(glm::mat4(),(5.f*front)+pos));
//			
//			const float rotationVel = 1.8f*time;
//#ifdef EULER
//
//			{
//				static auto rotation = glm::vec3(0, 0, 0);
//				if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//					rotation.x += rotationVel;
//				}
//				if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//					rotation.x -= rotationVel;
//
//				}
//				if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//					rotation.y += rotationVel;
//
//				}
//				if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//					rotation.y -= rotationVel;
//				}
//				currentlyMainpulatedObject->setRotation(glm::eulerAngleXYZ(rotation.x, rotation.y, 0.f));
//
//			}
//#else
//			{
//				auto& rotation = glm::quat_cast(currentlyMainpulatedObject->getRotation());
//				if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//					rotation *= glm::quat(cos(rotationVel / 2.f), sin(rotationVel / 2.f), 0, 0);
//				}
//				if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//					rotation *= glm::quat(cos(-rotationVel / 2.f), sin(-rotationVel / 2.f), 0, 0);
//				}
//				if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//					rotation *= glm::quat(cos(-rotationVel / 2.f), 0, sin(-rotationVel / 2.f), 0);
//				}
//				if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//					rotation *= glm::quat(cos(rotationVel / 2.f), 0, sin(rotationVel / 2.f), 0);
//				}
//				rotation = glm::normalize(rotation);
//
//				currentlyMainpulatedObject->setRotation(glm::mat4_cast(rotation));
//			}
//#endif
//
//		}
//
//



	//	pyramid2.draw(camera);

	}
private:

	TextureModelPrototype awesomeFaceProto= createSpritePrototype("awesomeface.png");
	Sprite sprite = awesomeFaceProto.createInstance();

	TextureModelPrototype singlePiramidProto = loadModel("pyramid.dae", "pyramid.png");
	TextureModelPrototype domekProto = loadModel("domek.dae", "domek.jpg");

	Model pyramid = singlePiramidProto.createInstance();
	Stearing stearing{ &pyramid.getModel() };
	//Model pyramid = loadModel("pyramid.dae", "pyramid.png");
	TextureModelPrototype pyramid2Proto = loadModel("pir.dae","pir.png");
	TextureModelPrototype neptunProto = loadModel("neptun.dae", "neptun.jpg");

	SpiralPrototype spiralProto = createSpiral();
	

	LigthModelPrototype lProto = loadModelLigth("pir.dae", "pir.png");

	SkyBox skyBox = createSkyBox();
	Cone cone = createCone();

	ComponentHandle<Camera> camera;

	GLFWwindow * window;
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

	Input::init(window);
	Renderer r(window);
	r.loop();

    return 0;
}

