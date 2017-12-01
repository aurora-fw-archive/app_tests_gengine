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
GEngine::GLProgram *sunprogram;

double mx, my;

afwslot slot_Window_on_render(GEngine::Window* window, GEngine::InputManager* inputHandler) {
	Debug::Log(window->getWidth(), "*", window->getHeight());
	inputHandler->getMousePosition(mx, my);
	Debug::Log(mx, ", ", my);
	sunprogram->setUniform2f("light_pos", Math::Vector2D((float)(mx * 0.5f / window->getWidth()), (float)(0.5f - my * 0.5f / window->getHeight())));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

afwslot slot_MyApp_on_open()
{
	GEngine::Window* window = new GEngine::Window(MyGApp, "Testing GEngine", GEngine::WindowProperties(600, 600, false));
	GEngine::InputManager* inputHandler = new GEngine::InputManager(window);

	CLI::Log(CLI::Information, "OpenGL Version: ", GEngine::getGLVersion());
	CLI::Log(CLI::Information, "GLSL Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};
	GLfloat cube_vertices[]=
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

	GLuint vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	GLuint vbo;
	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	
	GEngine::GLShader *sunshader_vert = new GEngine::GLShader(GEngine::Vertex, "Clouds");
	GEngine::GLShader *sunshader_frag = new GEngine::GLShader(GEngine::Fragment, "Clouds");
	sunshader_vert->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.vert").c_str());
	sunshader_frag->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.frag").c_str());

	sunprogram = new GEngine::GLProgram("Clouds");
	sunprogram->addShader(sunshader_vert);
	sunprogram->addShader(sunshader_frag);
	sunprogram->generate();
	delete sunshader_frag;
	delete sunshader_vert;
	sunprogram->enable();
	Math::Matrix4x4 ortho = Math::Matrix4x4::orthographic(0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f);
	sunprogram->setUniformMat4("pr_matrix", ortho);
	sunprogram->setUniformMat4("ml_matrix", Math::Matrix4x4::translate(Math::Vector3D(0, 0, 0)));
	sunprogram->setUniform2f("light_pos", Math::Vector2D(0.5f, 0.5f));
	GEngine::ColorF pointerColor = GEngine::ColorF(GEngine::CommonColor::Brown);
	sunprogram->setUniform4f("colour", Math::Vector4D(pointerColor.r, pointerColor.g, pointerColor.b, pointerColor.a));
	GEngine::ColorF backgroundColor = GEngine::ColorF(GEngine::CommonColor::Yellow);
	GLCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));

	while(!window->isClosed())
	{
		window->clear();
		slot_Window_on_render(window, inputHandler);
		window->update();
	}
	Application::ExitSuccess();
}

int main(int argc, char * argv[])
{
	MyApp = new Application(slot_MyApp_on_open, argc, argv);
	delete MyApp;
	return 0;
}
