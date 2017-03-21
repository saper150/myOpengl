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
	void setTranslation(const glm::mat4& translation) {
		_translation = translation;
		updateModel();
	}
	void setScale(const glm::mat4& scale) {
		_scale = scale;
		updateModel();
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