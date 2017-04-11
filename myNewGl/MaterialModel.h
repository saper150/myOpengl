#pragma once

#include "Colider.h"
struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
};

enum Materials {
	PEARL = 0
};



Material createMaterial(Materials material);


struct MaterialPoint {
	glm::vec3 pos;
	glm::vec3 normal;
};



struct MaterialModelPrototype {
	MaterialModelPrototype(const std::vector<MaterialPoint>& points,const Material& material)
		:colider(createColider<MaterialPoint>(points)), 
		programDescription(Shaders::getInstance().getProgram(PROGRAMS::MATERIAL_PROGRAM)),
		material(material)
	{

		size = points.size();
		vertexArray = createVertexArray();
		vertexBuffer = createArrayBuffer(points);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MaterialPoint), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MaterialPoint), (GLvoid*)(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);

	}

	class Instance {
	public:
		Instance(const MaterialModelPrototype* parent) :parent(parent)
		{}
		void draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
			glUseProgram(parent->programDescription->program);


			auto& locations = parent->programDescription->uniformLocations;
			glUniformMatrix4fv(locations.at("projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(locations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(locations.at("model"), 1, GL_FALSE, glm::value_ptr(model));


			glUniform3fv(locations.at("material.ambient"), 1, glm::value_ptr(parent->material.ambient));
			glUniform3fv(locations.at("material.diffuse"), 1, glm::value_ptr(parent->material.diffuse));
			glUniform3fv(locations.at("material.specular"), 1, glm::value_ptr(parent->material.specular));
			glUniform1f(locations.at("material.shininess"),parent->material.shininess);

			glBindVertexArray(parent->vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, parent->size);
			glBindVertexArray(0);
		}
	private:
		const MaterialModelPrototype* parent;
	};

	MaterialModelPrototype::Instance createInstance() const { return Instance(this); }
	GLsizei size = 0;
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint texture;
	const ProgramDescription * programDescription;
	aabbColider colider;
	Material material;

};