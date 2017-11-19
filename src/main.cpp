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
#include <AuroraFW/GEngine/Color.h>
#include <AuroraFW/GEngine/GL/Shader.h>
#include <AuroraFW/GEngine/GL/Program.h>
#include <AuroraFW/IO/File.h>

using namespace AuroraFW;

Application *MyApp;
GEngine::Application MyGApp("Test GEngine", GEngine::GraphicsAPI::OpenGL);
GEngine::GLProgram *sunprogram;

GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 0.0f};  /* White diffuse light. */
GLfloat light_position[] = {1.0f, 3.0f, 1.0f, 0.2f};  /* Infinite light location. */
GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
	{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
	{0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
	{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */

double mx, my;

afwslot slot_Window_on_render(GEngine::Window* window, GEngine::InputManager* inputHandler) {
	CLI::Output << window->getWidth() << "*" << window->getHeight() << CLI::EndLine;
	inputHandler->getMousePosition(mx, my);
	CLI::Output << mx << ", " << my << CLI::EndLine;
	//DEBUG ONLY!!!
	/*
	glBegin(GL_TRIANGLES);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(0.5f, -0.5f);
	glEnd();
	int i;

	for (i = 0; i < 6; i++) {
	glBegin(GL_QUADS);
	glNormal3fv(&n[i][0]);
	glVertex3fv(&v[faces[i][0]][0]);
	glVertex3fv(&v[faces[i][1]][0]);
	glVertex3fv(&v[faces[i][2]][0]);
	glVertex3fv(&v[faces[i][3]][0]);
	glEnd();
	}
	glDrawArrays(GL_ARRAY_BUFFER, 0, 6);
	*/
	sunprogram->setUniform2f("light_pos", Math::Vector2D((float)(mx * 0.5f / 600.0f), (float)(0.5f - my * 0.5f / 600.0f)));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

afwslot slot_MyApp_on_open()
{
	GEngine::Window* window = new GEngine::Window(MyGApp, "Testing GEngine", GEngine::WindowProperties(600, 600, false, true));
	GEngine::InputManager* inputHandler = new GEngine::InputManager(window);

	CLI::Log(CLI::Information, "OpenGL Version: ", GEngine::getGLVersion());

	GLfloat vertices[] = 
	{
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
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
	GEngine::ColorF backgroundColor = GEngine::ColorF(GEngine::CommonColor::Tomato);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	#if(0)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//...

	/* Setup cube vertex data. */
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;540
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	/* Enable a single OpenGL light. */  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 45.0,
	/* aspect ratio */ (4.0f/3.0f),
	/* Z near */ 1.0, /* Z far */ 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(1.5f, 0.5f, 5.5f,  /* eye is at (0,0,5) */
	0.0f, 0.0f, 0.0f,      /* center is at (0,0,0) */
	0.0f, 1.0f, 0.0f);      /* up is in positive Y direction */

	/* Adjust cube position to be asthetic angle. */
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(45, 0.0, 0.0, 1.0);
	#endif

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
