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
GEngine::Application MyGApp("Test GEngine", GEngine::GraphicsAPI::OpenGL);
GEngine::WindowProperties wp = GEngine::WindowProperties(600, 600, false);
GEngine::Window *window;
GEngine::InputManager *inputHandler;
GEngine::GLProgram *sunprogram;
IO::Timer MyTimer = IO::Timer();

Math::Vector4D color_vec;

double mx, my;
float sunsize = 0.05f;

afwslot slot_Window_on_render() {
	inputHandler->getMousePosition(mx, my);
	if(inputHandler->isKeyPressed(GLFW_KEY_INSERT)) {
		sunsize += 0.001f;
		sunprogram->setValue("size", sunsize);
	}
	if(inputHandler->isKeyPressed(GLFW_KEY_DELETE)) {
		sunsize -= 0.001f;
		sunprogram->setValue("size", sunsize);
	}

	if(inputHandler->isKeyPressed(GLFW_KEY_1)) {
		if(inputHandler->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) color_vec.x -= 0.01f;
		else color_vec.x += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	if(inputHandler->isKeyPressed(GLFW_KEY_2)) {
		if(inputHandler->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) color_vec.y -= 0.01f;
		else color_vec.y += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	if(inputHandler->isKeyPressed(GLFW_KEY_3)) {
		if(inputHandler->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) color_vec.z -= 0.01f;
		else color_vec.z += 0.01f;
		sunprogram->setValue("colour", color_vec);
	}

	sunprogram->setValue("light_pos", Math::Vector2D((float)(mx * 0.5f / window->getWidth()), (float)(0.5f - my * 0.5f / window->getHeight())));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

afwslot slot_MyApp_on_open()
{
	window = new GEngine::Window(MyGApp, "Testing GEngine", wp);
	inputHandler = new GEngine::InputManager(window);

	CLI::Log(CLI::Information, "OpenGL Version: ", GEngine::getGLVersion());
	CLI::Log(CLI::Information, "OpenGL Renderer: ", glGetString(GL_RENDERER));
	CLI::Log(CLI::Information, "GLSL Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	/*
	GLfloat cube_vertices[] =
	{
		 1.f,  1.f,  1.f,	1.0f, 0.0f, 0.0f, //0
		-1.f,  1.f,  1.f,	0.0f, 1.0f, 0.0f, //1
		-1.f,  1.f, -1.f,	0.0f, 0.0f, 1.0f, //2
		 1.f,  1.f, -1.f,	1.0f, 1.0f, 1.0f, //3
		 1.f, -1.f,  1.f,	1.0f, 1.0f, 0.0f, //4
		-1.f, -1.f,  1.f,	1.0f, 1.0f, 1.0f, //5
		-1.f, -1.f, -1.f,	0.0f, 1.0f, 1.0f, //6
		 1.f, -1.f, -1.f,	1.0f, 0.0f, 1.0f  //7
	};
	GLuint cube_indices[] =
	{
		0, 1, 3, //top 1
		3, 1, 2, //top 2
		2, 6, 7, //front 1
		7, 3, 2, //front 2
		7, 6, 5, //bottom 1
		5, 4, 7, //bottom 2
		5, 1, 4, //back 1
		4, 1, 0, //back 2
		4, 3, 7, //right 1
		3, 4, 0, //right 2
		5, 6, 2, //left 1
		5, 1, 2  //left 2
	};
	*/

	GLuint vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	GLuint vbo;
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	
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
	GLCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));

	while(!window->isClosed())
	{
		MyTimer.reset();
		window->clear();
		slot_Window_on_render();
		window->update();
		Debug::Log("FPS:\t", (1000/MyTimer.elapsedMillis()));
		Debug::Log("Size:\t", window->getWidth(), "*", window->getHeight());
		Debug::Log("Input:\t", mx, ", ", my);
	}
	Application::ExitSuccess();
}

int main(int argc, char * argv[])
{
	MyApp = new Application(slot_MyApp_on_open, argc, argv);
	delete MyApp;
	return 0;
}
