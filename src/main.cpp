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

#include <AuroraFW/Aurora.h>

using namespace AuroraFW;

Application *MyApp;
GEngine::Application MyGApp("Test GEngine", GEngine::API::OpenGL);
GEngine::WindowProperties wp = GEngine::WindowProperties(600, 600, false);
GEngine::Window *window;
GEngine::Renderer *renderer;
GEngine::InputManager *inputHandler;
GEngine::GLProgram *sunprogram;
IO::Timer MyTimer = IO::Timer();

Math::Vector4D color_vec;

double mx, my;
float sunsize = 0.05f;

GEngine::GLBuffer* ibo;
GEngine::GLVertexArray* pointSprite;

afwslot slot_Window_on_render() {
	inputHandler->getMousePosition(mx, my);
	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_INSERT)) {
		sunsize += 0.001f;
		sunprogram->setValue("size", sunsize);
	}
	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_DELETE)) {
		sunsize -= 0.001f;
		sunprogram->setValue("size", sunsize);
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_1)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.x -= 0.01f;
		else color_vec.x += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_2)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.y -= 0.01f;
		else color_vec.y += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_3)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.z -= 0.01f;
		else color_vec.z += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	sunprogram->setValue("light_pos", Math::Vector2D((float)(mx * 0.5 / window->getWidth()), (float)(0.5 - my * 0.5 / window->getHeight())));

	pointSprite->bind();
	ibo->bind();
	GLCall(glDrawElements(GL_TRIANGLES, ibo->size()/(sizeof(ushort)), GL_UNSIGNED_SHORT, AFW_NULL));
	ibo->unbind();
	pointSprite->unbind();
}

afwslot slot_MyApp_on_open()
{
	wp.samples = 16;
	wp.doubleBuffer = true;
	wp.vsync = false;
	window = new GEngine::Window("Testing GEngine", wp);
	inputHandler = new GEngine::InputManager(window);
	renderer = GEngine::Renderer::Load();

	CLI::Log(CLI::Information, "OpenGL Version: ", GEngine::GL::getVersion());
	CLI::Log(CLI::Information, "OpenGL Vendor: ", glGetString(GL_VENDOR));
	CLI::Log(CLI::Information, "OpenGL Renderer: ", glGetString(GL_RENDERER));
	CLI::Log(CLI::Information, "GLSL Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));
	int samples;
	GLCall(glGetIntegerv(GL_SAMPLES, &samples));
	CLI::Log(CLI::Information, "Samples: ", samples);

	GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		0.5f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f
	};

	GLushort indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	ibo = new GEngine::GLBuffer(GEngine::GL::IndexBuffer, sizeof(indices), indices);

	pointSprite = new GEngine::GLVertexArray();
	GEngine::GLVertexBufferLayout spriteLayout;
	spriteLayout.push<float>(3);
	pointSprite->addBuffer(GEngine::GLBuffer(GEngine::GL::Array, sizeof(vertices), vertices), spriteLayout);

	GEngine::GLShader *sunshader_vert = new GEngine::GLShader(GEngine::Vertex);
	GEngine::GLShader *sunshader_frag = new GEngine::GLShader(GEngine::Fragment);
	sunshader_vert->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.vert").c_str());
	sunshader_frag->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.frag").c_str());

	sunprogram = new GEngine::GLProgram();
	sunprogram->addShader(sunshader_vert);
	sunprogram->addShader(sunshader_frag);
	sunprogram->generate();
	delete sunshader_frag;
	delete sunshader_vert;
	sunprogram->enable();
	Math::Matrix4x4 ortho = Math::Matrix4x4::orthographic(0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f);
	sunprogram->setValue("pr_matrix", ortho);
	sunprogram->setValue("ml_matrix", Math::Matrix4x4::translate(Math::Vector3D(0, 0, 0)));
	sunprogram->setValue("light_pos", Math::Vector2D(0.5f, 0.5f));
	sunprogram->setValue("size", 0.05f);
	GEngine::ColorF pointerColor = GEngine::ColorF(GEngine::CommonColor::Brown);
	color_vec = Math::Vector4D(pointerColor.r, pointerColor.g, pointerColor.b, pointerColor.a);
	sunprogram->setValue("colour", color_vec);
	GEngine::ColorF backgroundColor = GEngine::ColorF(GEngine::CommonColor::Yellow);
	GEngine::GL::clearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	while(!window->isClosed())
	{
		MyTimer.reset();
		renderer->clear(GEngine::RENDERER_BUFFER_COLOR | GEngine::RENDERER_BUFFER_DEPTH);
		slot_Window_on_render();
		window->update();
		DebugManager::Log("FPS:\t", (1000/MyTimer.elapsedMillis()));
		DebugManager::Log("Size:\t", window->getWidth(), "*", window->getHeight());
		DebugManager::Log("Input:\t", mx, ", ", my);
	}
	Application::ExitSuccess();
}

int main(int argc, char * argv[])
{
	MyApp = new Application(slot_MyApp_on_open, argc, argv);
	delete MyApp;
	return 0;
}
