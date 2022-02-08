#include "AudioSDLWindow.h"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_sdl.h>

#include "Audio/Logger.h"

#undef main

#define SDL_MAIN_HANDLED

AudioSDLWindow::AudioSDLWindow() = default;

int AudioSDLWindow::CreateWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("[ERROR] %s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_GLSLversion = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	constexpr SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_Window = SDL_CreateWindow(
		m_WindowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		window_flags);

	m_AudioWindow = AudioImGuiWindow(m_Window, &m_AudioSystem);
	SDL_SetWindowMinimumSize(m_Window, 500, 300);

	m_GLContext = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_GLContext);

	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		logger::LogError("Couldn't initialize GLAD.");
		return -1;
	}
	logger::LogInfo("Initialized GLAD.");

	glViewport(0, 0, windowWidth, windowHeight);

	m_AudioWindow.CreateImGui(m_GLContext, m_GLSLversion.c_str());

	ImVec4 background = ImVec4(35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f);

	glClearColor(background.x, background.y, background.z, background.w);
	while (m_Running)
	{
		RenderWindow();
	}
	logger::LogInfo("Main loop ended.");
	return 0;
}

void AudioSDLWindow::RenderWindow()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
			{
				m_Running = false;
				break;
			}
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
					{
						windowWidth = event.window.data1;
						windowHeight = event.window.data2;
						glViewport(0, 0, windowWidth, windowHeight);
						break;
					}
					default:
					{
						break;
					}
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					{
						m_Running = false;
						break;
					}
					default:
					{
						break;
					}
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	m_AudioSystem.Update();
	m_AudioWindow.RenderImGui();

	SDL_GL_SwapWindow(m_Window);
}

void AudioSDLWindow::DeleteWindow()
{
	m_AudioWindow.DeleteWindow();

	SDL_GL_DeleteContext(m_GLContext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}