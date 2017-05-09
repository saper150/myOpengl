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
#include "LightSource.h"


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

struct PointPrototype {
	PointPrototype(GLuint program, const std::vector<SizePoint>& points) :program(program)
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
		Instance(PointPrototype* parent):parent(parent)
		{

		}
		void draw(const glm::mat4& mvp) {
			glUseProgram(parent->program);
			glUniformMatrix4fv(parent->MVPplcation, 1, GL_FALSE, glm::value_ptr(mvp));

			glBindVertexArray(parent->vertexArray);
			glDrawArrays(GL_TRIANGLE_FAN, 0, parent->size);
			glBindVertexArray(0);
		}

	private:
		PointPrototype* parent;
	};

	PointPrototype::Instance createInstance() {
		return Instance(this);
	}

	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;

	GLuint MVPplcation;
	GLuint program;
};


std::array<PointPrototype,2> createCone() {
	glm::vec3 color1 = { 0,1,0 };
	glm::vec3 color2 = { 1,0,0 };



	std::array<std::vector<SizePoint>,2> allPoints;


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
		allPoints[0] = points;

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
		allPoints[1] = points;

	}

	return{
		PointPrototype(Shaders::getInstance().getProgram(Shaders::vertex::FLAT_POINT, Shaders::fragment::FLAT_COLOR),allPoints[0]),
		PointPrototype(Shaders::getInstance().getProgram(Shaders::vertex::FLAT_POINT, Shaders::fragment::FLAT_COLOR),allPoints[1]),
	};

}




GLFWwindow* Input::_window;
struct DrawingSystem : public System<DrawingSystem> {

	ComponentHandle<Camera> camera;
	DrawingSystem(ComponentHandle<Camera> camera):camera(camera)
	{

	}
	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {

		es.each<Transformation, TextureModelPrototype::Instance>(
			[dt,this](Entity entity, Transformation &t, TextureModelPrototype::Instance &instance){
			const auto mvp = camera->projection*camera->view*t.createModel();
			instance.draw(mvp);
		});

		es.each<Transformation, LigthModelPrototype::Instance>(
			[dt, this](Entity entity, Transformation &t, LigthModelPrototype::Instance &instance) {
			instance.draw(camera->projection,camera->view,t.createModel());
		});
		es.each<Transformation, MaterialModelPrototype::Instance>(
			[dt, this](Entity entity, Transformation &t, MaterialModelPrototype::Instance &instance) {
			instance.draw(camera->projection, camera->view, t.createModel());
		});

		es.each<PointPrototype::Instance>(
			[dt, this](Entity entity, PointPrototype::Instance &instance) {
			const auto mvp = camera->projection*camera->view;
			instance.draw(mvp);
		});

		es.each<MaterialModelPrototype::Instance,std::shared_ptr<btRigidBody>>(
			[dt, this](Entity entity, MaterialModelPrototype::Instance &instance, std::shared_ptr<btRigidBody>& body ) {

			btTransform& trans = body->getWorldTransform();
			glm::mat4 model;
			trans.getOpenGLMatrix(glm::value_ptr(model));
			instance.draw(camera->projection, camera->view,model);
		});

		es.each<TextureModelPrototype::Instance, std::shared_ptr<btRigidBody>>(
			[dt, this](Entity entity, TextureModelPrototype::Instance &instance, std::shared_ptr<btRigidBody>& body) {

			btTransform& trans = body->getWorldTransform();
			glm::mat4 model;
			trans.getOpenGLMatrix(glm::value_ptr(model));
			instance.draw(camera->projection* camera->view* model);
		});

		es.each<std::shared_ptr<btRigidBody>, LigthModelPrototype::Instance>(
			[dt, this](Entity entity, std::shared_ptr<btRigidBody> &body, LigthModelPrototype::Instance &instance) {

			btTransform& trans = body->getWorldTransform();
			glm::mat4 model;
			trans.getOpenGLMatrix(glm::value_ptr(model));
			instance.draw(camera->projection, camera->view, model);

		});


		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		es.each<Transformation, TextureModelPrototype::Bilboard>(
			[dt, this](Entity entity, Transformation &position, TextureModelPrototype::Instance instance) {

			const auto& view = camera->view;
			glm::mat4 _model = position.createModel();

			_model[0][0] = view[0][0];
			_model[0][1] = view[1][0];
			_model[0][2] = view[2][0];
			_model[0][2] = view[2][0];
			_model[1][0] = view[0][1];
			_model[1][1] = view[1][1];
			_model[1][2] = view[2][1];
			_model[2][0] = view[0][2];
			_model[2][1] = view[1][2];
			_model[2][2] = view[2][2];


			auto mvp = camera->projection*camera->view*_model;

			instance.draw(mvp);
		});
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	};
};

struct SpiralSystem : public System<SpiralSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override {
		ComponentHandle<Camera> cameraHandle;
		Entity cameraEntity = *es.entities_with_components(cameraHandle).begin();
		ComponentHandle<Camera> camera = cameraEntity.component<Camera>();

		es.each<Transformation,SpiralPrototype::SpiralInstance, SpiralPosition>(
			[dt,&camera](Entity entity, Transformation &t, SpiralPrototype::SpiralInstance instance, SpiralPosition &position) {

			position.spiralposition -= static_cast<float>(dt)*position.spiralVel;
			if (position.spiralposition < 0.f) {
				position.spiralposition = position.spiralposition + 1.f;
			}

			const auto mvp = camera->projection*camera->view*t.createModel();
			instance.draw(mvp, position.spiralSize,position.spiralposition);

		});
	};

};


class Scene {
public:
	Scene(GLFWwindow * window):window(window)
	{

		{
			{
				Entity entity = entities.create();
				entity.assign<PointPrototype::Instance>(cone[0].createInstance());
			}

			{
				Entity entity = entities.create();
				entity.assign<PointPrototype::Instance>(cone[1].createInstance());
			}

		}


		{
			//pyramid
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 4,4,-4 };

			entity.component<Transformation>()->rotation = glm::quat({0.5,0.5,0});
			entity.assign<TextureModelPrototype::Instance>(singlePiramidProto.createInstance());
			entity.assign<aabbColider*>(&singlePiramidProto.colider);
		}
		{
			//domek
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position= randomVector()*8.f;
			entity.assign<TextureModelPrototype::Instance>(domekProto.createInstance());
			entity.assign<aabbColider*>(&domekProto.colider);
		}
		{
			//neptun
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = randomVector()*5.f;
			entity.assign<TextureModelPrototype::Instance>(neptunProto.createInstance());
			entity.assign<aabbColider*>(&neptunProto.colider);
		}
		{
			//awesome face sprite
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 0,0,0 };
			entity.component<Transformation>()->scale = 100.f;

			entity.component<Transformation>()->rotation = glm::quat(glm::eulerAngleX(glm::radians(90.f)));

			entity.assign<TextureModelPrototype::Instance>(awesomeFaceProto.createInstance());
			entity.assign<aabbColider*>(&awesomeFaceProto.colider);
		}

		for(int i = 0; i < 3; i++)
		{
			//palms
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 3,-9,6 };
			entity.assign<TextureModelPrototype::Instance>(palmProto.createInstance());
			entity.assign<aabbColider*>(&palmProto.colider);
		}

		{
			//old woman
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 3,-9,9 };
			entity.assign<TextureModelPrototype::Instance>(oldWoman.createInstance());
			entity.assign<aabbColider*>(&oldWoman.colider);
		}
		for (size_t i = 0; i < 10; i++)
		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = randomVector();
			entity.assign<Velocity>();
			entity.assign<Stearing::Separation>();
			entity.assign<TextureModelPrototype::Bilboard>(awesomeFaceProto.createBilboard());
			entity.assign<aabbColider*>(&awesomeFaceProto.colider);
		
		}

		{
			//spiral
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = randomVector()*5.f;
			entity.assign<SpiralPrototype::SpiralInstance>(spiralProto.createInstance());
			entity.assign<SpiralPosition>()->spiralSize;
			entity.assign<aabbColider*>(&spiralProto.colider);
			entity.component<SpiralPosition>()->spiralSize = spiralProto.totalSize;
		}
		{
			//my cone
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { -4,-5,-6 };
			entity.assign<Stearing::Flee>();
			entity.assign<LigthModelPrototype::Instance>(myCone.createInstance());
			entity.assign<aabbColider*>(&myCone.colider);
		}
		{
			//smooth pyraimd
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { -4,-5,10 };

			entity.assign<Stearing::SeekTarget>();
			entity.assign<LigthModelPrototype::Instance>(smoothPiramidProto.createInstance());
			entity.assign<aabbColider*>(&smoothPiramidProto.colider);
		}

		{
			//material cone
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 20,-5,-6 };
			entity.assign<MaterialModelPrototype::Instance>(materialProto.createInstance());
			entity.assign<aabbColider*>(&materialProto.colider);
		}

		{
			//material sphere
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 20,-5,6 };
			entity.assign<MaterialModelPrototype::Instance>(neptunMaterialProto.createInstance());
			entity.assign<aabbColider*>(&neptunMaterialProto.colider);
		}

		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { 10,3,6 };

			entity.assign<Light>();
			entity.component<Light>()->ambient = { 0.1f,0.1f,0.1f };
			entity.component<Light>()->diffuse= { 0.8f,0.8f,0.8f };
			entity.component<Light>()->specular= { 0.5f,0.5f,0.5f };
			entity.component<Light>()->linear = 1.0f;
			entity.component<Light>()->linear= 0.045f;
			entity.component<Light>()->quadratic = 0.0075f;
			entity.component<Transformation>()->position = { 3,-5,-6 };
			entity.assign<MaterialModelPrototype::Instance>(neptunMaterialProto.createInstance());
			entity.assign<aabbColider*>(&neptunMaterialProto.colider);
		}
		{
			Entity entity = entities.create();
			entity.assign<Transformation>();
			entity.component<Transformation>()->position = { -10,3,6 };

			entity.assign<Light>();
			entity.component<Light>()->ambient = { 0.1f,0.1f,0.1f };
			entity.component<Light>()->diffuse = { 0.8f,0.8f,0.8f };
			entity.component<Light>()->specular = { 0.5f,0.5f,0.5f };
			entity.component<Light>()->linear = 1.0f;
			entity.component<Light>()->linear = 0.045f;
			entity.component<Light>()->quadratic = 0.0075f;
			entity.component<Transformation>()->position = { 3,5,-6 };
			entity.assign<MaterialModelPrototype::Instance>(neptunMaterialProto.createInstance());
			entity.assign<aabbColider*>(&neptunMaterialProto.colider);
		}

		{
			Entity entity = entities.create();

			btStaticPlaneShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
			btDefaultMotionState groundMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
			btRigidBody::btRigidBodyConstructionInfo
				groundRigidBodyCI(0, &groundMotionState, groundShape, btVector3(0, 0, 0));
			

			std::shared_ptr<btRigidBody> body(new btRigidBody(groundRigidBodyCI), rigidBodyDeleater(physics.dynamicsWorld.get()));
			entity.assign<std::shared_ptr<btRigidBody>>(body);
			body->setUserPointer(nullptr);
			physics.dynamicsWorld->addRigidBody(body.get());

		}

		{
			Entity entity = entities.create();

			btSphereShape* fallShape = new btSphereShape(1);

			btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
			btScalar mass = 1;
			btVector3 fallInertia(100.f, 100.f, 100.f);
			fallShape->calculateLocalInertia(mass, fallInertia);
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);

			std::shared_ptr<btRigidBody> body(new btRigidBody(fallRigidBodyCI), rigidBodyDeleater(physics.dynamicsWorld.get()));
			entity.assign<std::shared_ptr<btRigidBody>>(body);
			entity.assign<LigthModelPrototype::Instance>(neptunLightproto.createInstance());

			std::cout << entity.id().id() << std::endl;
			std::bitset<64> x(entity.id().id());
			std::cout << x << std::endl;

			const auto a = static_cast<unsigned int>(entity.id().id());

			body->setUserPointer(reinterpret_cast<void*>(a));

			physics.dynamicsWorld->addRigidBody(body.get());

		}
		{
			Entity entity = entities.create();

			btSphereShape* fallShape = new btSphereShape(1);

			btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
			btScalar mass = 1;
			btVector3 fallInertia(100.f, 100.f, 100.f);
			fallShape->calculateLocalInertia(mass, fallInertia);
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);

			std::shared_ptr<btRigidBody> body(new btRigidBody(fallRigidBodyCI), rigidBodyDeleater(physics.dynamicsWorld.get()));
			entity.assign<std::shared_ptr<btRigidBody>>(body);
			entity.assign<MaterialModelPrototype::Instance>(neptunMaterialProto.createInstance());

			std::cout << entity.id().id() << std::endl;
			std::bitset<64> x(entity.id().id());
			std::cout << x << std::endl;

			const auto a = static_cast<unsigned int>(entity.id().id());

			body->setUserPointer(reinterpret_cast<void*>(a));

			physics.dynamicsWorld->addRigidBody(body.get());

		}
		btBoxShape* s = new btBoxShape({ 1,1,1 });


		Entity camera = entities.create();
		camera.assign<Position>();
		camera.assign<CameraDirection>();
		camera.assign<Camera>();
		this->camera = camera.component<Camera>();


		systems.add<DrawingSystem>(camera.component<Camera>());

		const std::vector<ProgramDescription*> cameraPositionDescriptions = {
			Shaders::getInstance().getProgram(PROGRAMS::LIGTH_PROGRAM),
			Shaders::getInstance().getProgram(PROGRAMS::MATERIAL_PROGRAM)
		};
		systems.add<CameraSystem>(camera, cameraPositionDescriptions,physics.dynamicsWorld.get(),&ex);

		const std::vector<ProgramDescription*> lightDescriptions = {
			Shaders::getInstance().getProgram(PROGRAMS::LIGTH_PROGRAM),
			Shaders::getInstance().getProgram(PROGRAMS::MATERIAL_PROGRAM)
		};
		systems.add<LightSystem>(lightDescriptions);

		systems.add<InputSystem>(window);
		systems.add <SpiralSystem>();

		systems.add<VelocitySystem>();
		systems.add<StearingSystem>();

		systems.configure();

	}

	void updata(float time) {

		physics.symulate(time);
		skyBox.draw(camera);
		systems.update<InputSystem>(time);
		systems.update<CameraSystem>(time);
		systems.update<VelocitySystem>(time);
		systems.update<StearingSystem>(time);
		systems.update<LightSystem>(time);
		systems.update<DrawingSystem>(time);
		systems.update<SpiralSystem>(time);
	}
private:
	Physics physics;

	EntityX ex;

	EntityManager& entities = ex.entities;
	SystemManager& systems = ex.systems;
	EventManager& events = ex.events;

	TextureModelPrototype awesomeFaceProto= createSpritePrototype("textures/awesomeface.png");
	TextureModelPrototype palmProto = createSpritePrototype("textures/palm.png");
	TextureModelPrototype oldWoman = createSpritePrototype("textures/old_woman.jpg");



	TextureModelPrototype singlePiramidProto = loadModel("models/pyramid.dae", "textures/pyramid.png");

	TextureModelPrototype domekProto = loadModel("models/domek.dae", "textures/domek.jpg");

	TextureModelPrototype pyramid2Proto = loadModel("models/pir.dae","textures/pir.png");
	TextureModelPrototype neptunProto = loadModel("models/neptun.dae", "textures/neptun.jpg");
	MaterialModelPrototype neptunMaterialProto = loadAsMaterial("models/neptun.dae", Materials::PEARL);

	std::array<PointPrototype, 2> cone = createCone();

	SpiralPrototype spiralProto = createSpiral();

	LigthModelPrototype neptunLightproto = loadModelLigth("models/neptun.dae", "textures/neptun.jpg");

	LigthModelPrototype myCone = loadModelLigth("models/pir.dae", "textures/pir.png");
	LigthModelPrototype smoothPiramidProto = loadModelLigth("models/pyramidSmooth.dae", "textures/pyramid.png");

	MaterialModelPrototype materialProto = loadAsMaterial("models/pir.dae", Materials::PEARL);

	SkyBox skyBox = createSkyBox();
	//Cone cone = createCone();

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
			static int frameCount = 0;

			static std::chrono::duration<float> timeToSecond;

			static auto prevTime = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			const auto frameDuration = static_cast<std::chrono::duration<float>> (now - prevTime);
			prevTime = std::chrono::high_resolution_clock::now();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene.updata(frameDuration.count());

			timeToSecond += frameDuration;
			if (timeToSecond > std::chrono::seconds(1)) {
				timeToSecond -= std::chrono::seconds(1);
				std::cout << "fps: " << frameCount << "\n";
				frameCount = 0;
			}
			frameCount++;

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


	try {

		Input::init(window);
		Renderer r(window);
		r.loop();
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		int i;
		std::cin>>i;
	}

    return 0;
}

