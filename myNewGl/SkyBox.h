#pragma once

class ProgramSkyBox {
public:
	ProgramSkyBox(GLuint program);
	void draw(glm::mat4 mvp);
	void record(const std::array<std::string, 6>& faces);
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
	void draw(const Camera& camera);

private:
	ProgramSkyBox program;

};

