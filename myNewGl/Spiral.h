#pragma once
#include "stdafx.h"
#include "Wraper.h"

struct Point3d {
	glm::vec3 pos;
	glm::vec3 color;
};

struct SpiralPrototype {
	SpiralPrototype(GLuint program, const std::vector<Point3d>& points,float totalSize) :program(program),totalSize(totalSize)
	{
		MVPplcation = glGetUniformLocation(program, "MVP");
		totalSizeLocation = glGetUniformLocation(program, "totalSize");
		spiralPositionLocation = glGetUniformLocation(program, "spiralPosition");

		size = points.size();

		vertexArray = createVertexArray();
		vertexBuffer = createArrayBuffer(points);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
		class SpiralInstance {
		public:
			SpiralInstance(GLuint program,GLuint MVPplcation,GLuint totalSizeLocation,GLuint spiralPositionLocation,GLint size,GLuint vertexArray)
				:program(program),MVPplcation(MVPplcation),totalSizeLocation(totalSizeLocation),spiralPositionLocation(spiralPositionLocation),size(size),vertexArray(vertexArray)
			{
				
			}

			void draw(const glm::mat4& mvp, GLfloat totalSize = 0.f, GLfloat spiralPosition = 0.f) {
				glUseProgram(program);
				glUniformMatrix4fv(MVPplcation, 1, GL_FALSE, glm::value_ptr(mvp));
				glUniform1f(spiralPositionLocation, spiralPosition);
				glUniform1f(totalSizeLocation, totalSize);

				glBindVertexArray(vertexArray);
				glDrawArrays(GL_POINTS, 0, size);
				glBindVertexArray(0);
			}

	private:
		
		GLuint vertexArray;
		GLuint MVPplcation;
		GLuint totalSizeLocation;
		GLuint spiralPositionLocation;
		GLuint program;
		GLint size;

	};

	SpiralInstance createInstance() {
		return SpiralInstance(program, MVPplcation, totalSizeLocation, spiralPositionLocation, size, vertexArray);
	}
	float getTotalSize() { return totalSize; };
	float totalSize;
	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;

	GLuint MVPplcation;
	GLuint totalSizeLocation;
	GLuint spiralPositionLocation;
	GLuint program;
};

class SpiralWarper :public SingleTransformationWraper<SpiralPrototype::SpiralInstance>
{
public:
	SpiralWarper(const SpiralPrototype::SpiralInstance& instance, float totalSize) :SingleTransformationWraper(instance), totalSize(totalSize)
	{

	}
	void update(float time) {
		spiralPosition += time*spiralVel;
		if (spiralPosition > 1.f) {
			spiralPosition = spiralPosition - 1.f;
		}
	}
	void draw(const Camera& camera) override {
		auto mvp = camera.projection()*camera.view()*_model;
		program.draw(mvp, totalSize, spiralPosition);

	}

private:
	float spiralVel = 0.02;
	float spiralPosition = 0;
	float totalSize;

};

SpiralPrototype createSpiral() {
	std::vector<Point3d> points = {};
	float y = 0;
	for (float i = 0; i < PI * 2 * 11; i += 0.1f) {

		points.push_back({ { sin(i),y,cos(i) },{ 0.0,1.0,0.0 } });
		y += 0.01f;
	}
	return SpiralPrototype (Shaders::getInstance().getProgram(Shaders::vertex::SPIRAL, Shaders::fragment::IN_OUT_COLOR), points,y);
}