#pragma once
#include "stdafx.h"

struct TransformationWraper {
	TransformationWraper()
	{

	}
	void setRotation(const glm::mat4& rotation) {
		_rotation = rotation;
		updateModel();
	}
	const glm::mat4& getRotation() const{
		return _rotation;
	}
	void setTranslation(const glm::mat4& translation) {
		_translation = translation;
		updateModel();
	}
	void setScale(const glm::mat4& scale) {
		_scale = scale;
		updateModel();
	}
	const glm::mat4& const getModel() {
		return _model;
	}

protected:
	void updateModel() {
		_model = _translation * _rotation * _scale;
	}

	glm::mat4 _translation;
	glm::mat4 _rotation;
	glm::mat4 _scale;
	glm::mat4 _model;
};

template<typename T>
class SingleTransformationWraper :public TransformationWraper 
{
public:
	SingleTransformationWraper(const T& program):program(program){
			
	}
	virtual void draw(const Camera& camera) {
		auto mvp = camera.projection()*camera.view()*_model;
		program.draw(mvp);
	}
protected:
	T program;
};

template<typename T>
class Bilboard:public SingleTransformationWraper<T> 
{
public:
	Bilboard(const T& program):SingleTransformationWraper(program){}

	void draw(const Camera& camera) override {
		auto& view = camera.view();
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


		auto mvp = camera.projection()*view*_model;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glDepthMask(GL_FALSE);
		program.draw(mvp);
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
};

template<typename T>
class MultipleTransformationWraper :public TransformationWraper
{
public:
	MultipleTransformationWraper(const std::vector<T>& programs) :programs(programs)
	{

	}
	virtual void draw(const Camera& camera) {
		auto mvp = camera.projection()*camera.view()*_model;
		for (auto& program : programs) {
			program.draw(mvp);
		}
	}
protected:
	std::vector<T> programs;
};