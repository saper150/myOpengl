#pragma once

class ProgramSkyBox {
public:
	ProgramSkyBox(GLuint program, const std::array<std::string, 6>& faces);
	void draw(glm::mat4 mvp);

private:
	GLuint program;
	GLuint MVPLocation;
	GLuint texture;
	GLuint testureSampler;
	GLuint vertexArray;
	GLuint vertexBuffer;

};


class SkyBox
{
public:
	SkyBox(const std::array<std::string,6>& faces);
	~SkyBox();
	void draw(const ComponentHandle<Camera>& camera);

private:
	ProgramSkyBox program;

};

