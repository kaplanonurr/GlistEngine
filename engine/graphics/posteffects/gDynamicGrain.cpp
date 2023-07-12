/*
 * gDynamicGrain.cpp
 *
 *  Created on: 11 Tem 2023
 *      Author: Batuhan Yigit
 */

#include "gDynamicGrain.h"

gDynamicGrain::gDynamicGrain(float intensity, float speed) {
	shader = new gShader();
	shader->loadProgram(getVertSrc(), getFragSrc());
	shader->use();
	shader->setFloat("intensity", intensity);

	counter = 0.0;
	this->speed = speed;
}

gDynamicGrain::~gDynamicGrain() {
	delete shader;
}

void gDynamicGrain::use() {
	shader->use();
	counter += speed;
	shader->setFloat("counter", counter);
}

const std::string gDynamicGrain::getVertSrc() {
	const char* shadersource =
			"#version 330 core\n"
			"layout (location = 0) in vec2 aPos;"
			"layout (location = 1) in vec2 aTexCoords;"
			""
			"out vec2 TexCoords;"
			""
			"void main()"
			"{"
			"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);"
			"    TexCoords = aTexCoords;"
			"}\n";
	return std::string(shadersource);
}

const std::string gDynamicGrain::getFragSrc() {
	const char* shadersource =
			"#version 330 core\n"
			"out vec4 FragColor;"
			""
			"in vec2 TexCoords;"
			""
			"uniform sampler2D screenTexture;"
			"uniform float counter;"
			"uniform float intensity;"
			""
			"void main()"
			"{"
			"	vec4 texcolor = texture(screenTexture, TexCoords);"
			"	float noise = (fract(sin(dot(TexCoords, vec2(12.9898,78.233))) * 43758.5453123 + counter));"  //Perlin noise calculation.
			"	FragColor = texcolor - noise * intensity;"
			"}";
	return std::string(shadersource);
}
