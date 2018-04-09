/****************************************************************************
** ┌─┐┬ ┬┬─┐┌─┐┬─┐┌─┐  ┌─┐┬─┐┌─┐┌┬┐┌─┐┬ ┬┌─┐┬─┐┬┌─
** ├─┤│ │├┬┘│ │├┬┘├─┤  ├┤ ├┬┘├─┤│││├┤ ││││ │├┬┘├┴┐
** ┴ ┴└─┘┴└─└─┘┴└─┴ ┴  └  ┴└─┴ ┴┴ ┴└─┘└┴┘└─┘┴└─┴ ┴
** A Powerful General Purpose Framework
** More information in: https://aurora-fw.github.io/
**
** Copyright (C) 2017 Aurora Framework, All rights reserved.
**
** This file is part of the Aurora Framework. This framework is free
** software; you can redistribute it and/or modify it under the terms of
** the GNU General Public License version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3 requirements will be
** met: https://www.gnu.org/licenses/gpl-3.0.html.
****************************************************************************/

#include <AuroraFW/Core/ApplicationContext.h>
#include <AuroraFW/Core/InputListener.h>
#include <AuroraFW/GEngine/GraphicsContext.h>
#include <AuroraFW/GEngine/API/IndexBuffer.h>
#include <AuroraFW/GEngine/API/VertexArray.h>
#include <AuroraFW/GEngine/API/VertexBuffer.h>
#include <AuroraFW/GEngine/API/Texture.h>
#include <AuroraFW/GEngine/API/RTShader.h>
#include <AuroraFW/GEngine/API/RTShaderPipeline.h>
#include <AuroraFW/CLI/Output.h>

using namespace AuroraFW;
using namespace GEngine;

class MyApplication : public ApplicationContext, InputListener, public GraphicsContext {
public:
	MyApplication(int& argc, char* argv[])
		: ApplicationContext("GEngine Application Test", argc, argv),
	GraphicsContext("GEngine Test", API::OpenGL)
	{
		addInputListener(this);
	}

	void onStart()
	{
		root->getRenderer()->setBlendFunction(API::Renderer::BlendFunction::SourceAlpha, API::Renderer::BlendFunction::OneMinusSourceAlpha);
		root->getRenderer()->setBlend(true);

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f
		};

		uint indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		
		API::VertexBuffer* spriteVBO = API::VertexBuffer::Load(vertices, sizeof(vertices));

		_ibo = API::IndexBuffer::Load(indices, 6);

		_textureSprite = API::VertexArray::Load();
		API::BufferLayout spriteLayout;
		spriteLayout.push<float>(2);
		spriteLayout.push<float>(2);

		_textureSprite->addBuffer(spriteVBO, &spriteLayout);

		API::RTShader* basicshader_vert = API::RTShader::Load(Shader::Type::Vertex, "apps/tests/gengine/rsrc/basic/glsl330_core/shader.vert", API::RTShader::Language::GLSL, API::RTShader::LangVersion::GLSL330_CORE);
		API::RTShader* basicshader_frag = API::RTShader::Load(Shader::Type::Fragment, "apps/tests/gengine/rsrc/basic/glsl330_core/shader.frag", API::RTShader::Language::GLSL, API::RTShader::LangVersion::GLSL330_CORE);

		_basicpipeline = API::RTShaderPipeline::Load({basicshader_vert, basicshader_frag});
		_basicpipeline->generate();
		_basicpipeline->bind();

		_basicpipeline->setValue("u_Color", Math::Vector4D(0.8f, 0.3f, 0.8f, 1.0f));
		_basicpipeline->setValue("u_Texture", 0);

		API::Texture* _sampleTexture = API::Texture::Load("apps/tests/gengine/rsrc/logo.png");
		_sampleTexture->bind();

		delete _sampleTexture;
		delete basicshader_frag;
		delete basicshader_vert;
	}

	void onRender()
	{
		renderDebugGUI();
		root->getRenderer()->draw(_textureSprite, _ibo, _basicpipeline);
	}

	bool keyPressed(const KeyboardEvent& e)
	{
		CLI::Output << e.key << CLI::EndLine;
		return true;
	}

	private:
		API::IndexBuffer* _ibo;
		API::VertexArray* _textureSprite;
		API::RTShaderPipeline* _basicpipeline;
};

int main(int argc, char* argv[])
{
	MyApplication app(argc, argv);
	app.start();
	app.renderLoop();
	app.close();
	return 0;
}