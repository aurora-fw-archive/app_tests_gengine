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

#include <AuroraFW/GEngine/ImGui/Loader.h>

using namespace AuroraFW;

GEngine::Window *window;
GEngine::Renderer *renderer;
GEngine::InputManager *inputHandler;
GEngine::GLProgram *sunprogram;
GEngine::ImGuiLoader *guiLoader;

GEngine::Application MyGApp(GEngine::API::OpenGL);
GEngine::WindowProperties wp(800, 600, false);
IO::Timer MyTimer = IO::Timer();

Math::Vector4D color_vec;

double mx, my;
float sunsize = 0.05f;

bool debugWindow = true, menuWindow = true;
IO::Timer framerateTimer = IO::Timer();
std::array<float, 64> framerate;
float tpf;

GEngine::GLBuffer* ibo;
GEngine::GLVertexArray* pointSprite;

afwslot slot_Window_on_render() {
	inputHandler->getMousePosition(mx, my);

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_INSERT)) {
		sunsize += 0.001f * (1000.0f / framerate.back());
	}
	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_DELETE)) {
		sunsize -= 0.001f * (1000.0f / framerate.back());
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_1)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.x -= 0.001f  * (1000.0f / framerate.back());
		else color_vec.x += 0.001f * (1000.0f / framerate.back());
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_2)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.y -= 0.001f * (1000.0f / framerate.back());
		else color_vec.y += 0.001f * (1000.0f / framerate.back());
	}

	if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_3)) {
		if(inputHandler->isKeyPressed(AFW_GENGINE_KEY_LEFT_SHIFT)) color_vec.z -= 0.001f  * (1000.0f / framerate.back());
		else color_vec.z += 0.001f * (1000.0f / framerate.back());
	}

	sunprogram->setValue("light_pos", Math::Vector2D((float)(mx * 0.5 / window->getWidth()), (float)(0.5 - my * 0.5 / window->getHeight())));

	pointSprite->bind();
	ibo->bind();
	GLCall(glDrawElements(GL_TRIANGLES, ibo->size()/(sizeof(ushort)), GL_UNSIGNED_SHORT, AFW_NULL));
	ibo->unbind();
	pointSprite->unbind();

	if(debugWindow)
	{
		ImGui::Begin("Debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate.back(), framerate.back());
		ImGui::PlotLines("FPS", framerate.data(), framerate.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(300,50));
		ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
		ImGui::End();
	}
	if(menuWindow)
	{
		ImGui::Begin("GEngine Menu");
		ImGui::ColorEdit4("Sun Color", (float*)&color_vec);
		ImGui::SliderFloat("Sun Size", &sunsize, 0.0f, 1.0f);
		ImGui::Separator();
		ImGui::End();
	}

	sunprogram->setValue("colour", color_vec);
	sunprogram->setValue("size", sunsize);
}

afwslot slot_MyApp_on_open(Application* obj)
{
	bool psy_mode = false;
	for (std::vector<std::string>::iterator i = obj->args->begin(); i != obj->args->end(); ++i)
	{
		if(*i == "--samples" || *i == "-s")
			wp.samples = std::stoi(*++i);
		else if(*i == "--vsync" || *i == "-vS")
			wp.vsync = static_cast<bool>(std::stoi(*++i));
		else if(*i == "--psy")
		{
			wp.doubleBuffer = static_cast<bool>(std::stoi(*++i));
			psy_mode = true;
		}
		else if(*i == "-db")
			wp.doubleBuffer = static_cast<bool>(std::stoi(*++i));
	}

	window = AFW_NEW GEngine::Window("Testing GEngine", wp);
	inputHandler = AFW_NEW GEngine::InputManager(window);
	renderer = GEngine::Renderer::Load();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	guiLoader = GEngine::ImGuiLoader::Load(window, inputHandler);

	ImGui::StyleColorsDark();

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

	ibo = AFW_NEW GEngine::GLBuffer(GEngine::GL::IndexBuffer, sizeof(indices), indices);

	pointSprite = AFW_NEW GEngine::GLVertexArray();
	GEngine::GLVertexBufferLayout spriteLayout;
	spriteLayout.push<float>(3);
	pointSprite->addBuffer(GEngine::GLBuffer(GEngine::GL::Array, sizeof(vertices), vertices), spriteLayout);

	GEngine::GLShader *sunshader_vert = AFW_NEW GEngine::GLShader(GEngine::Vertex);
	GEngine::GLShader *sunshader_frag = AFW_NEW GEngine::GLShader(GEngine::Fragment);
	sunshader_vert->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.vert").c_str());
	sunshader_frag->compileFromSource(IO::readFile("apps/tests/gengine/rsrc/sun.frag").c_str());

	sunprogram = AFW_NEW GEngine::GLProgram();
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

	inputHandler->addMouseButtonCallback([](GLFWwindow *, int btn, int action, int mods) {
		//std::cout << "*clicky*: " <<
		//	btn << ", " << action << ", " << mods << std::endl;
	});

	inputHandler->addKeyCallback([debugWindow, menuWindow](GLFWwindow* , int key, int , int action, int ) {
		if(key == AFW_GENGINE_KEY_F3 && action == AFW_GENGINE_INPUT_PRESS) {
			debugWindow = (debugWindow) ? false : true;
		}

		if(key == AFW_GENGINE_KEY_F1 && action == AFW_GENGINE_INPUT_PRESS) {
			menuWindow = (menuWindow) ? false : true;
		}
	});

	framerate.fill(0.0f);
	while (!window->isClosed())
	{
		MyTimer.reset();
		window->update();
		//renderer->setViewport(0, 0, window->getWidth(), window->getHeight());
		renderer->clear(GEngine::RENDERER_BUFFER_COLOR | GEngine::RENDERER_BUFFER_DEPTH);
		guiLoader->newFrame();

		if(psy_mode)
			GEngine::GL::clearColor(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), 1);
		else
			slot_Window_on_render();
		ImGui::Render();
		guiLoader->renderDrawLists(ImGui::GetDrawData());
		window->present();
		tpf = MyTimer.elapsedMillis();
		if (framerateTimer.elapsedMillis() > 100.0f)
		{
			framerateTimer.reset();
			std::rotate(framerate.begin(), framerate.begin()+1,framerate.end());
			framerate[framerate.size()-1] = 1000.0f / tpf;
		}
	}
	//delete sunprogram;
	guiLoader->Unload();
	ImGui::DestroyContext();

	Application::ExitSuccess();
}

int main(int argc, char * argv[])
{
	Application MyApp(argc, argv, slot_MyApp_on_open);
	return 0;
}
