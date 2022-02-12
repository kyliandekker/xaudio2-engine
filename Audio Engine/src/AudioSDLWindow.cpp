#include "AudioSDLWindow.h"

#include <iostream>

#include "Audio/Logger.h"
#include <glad/glad.h>

AudioSDLWindow::AudioSDLWindow()
{
	CreateSDLWindow();
	CreateContext();
	CreateGlad();
	CreateImGui();
}

AudioSDLWindow::~AudioSDLWindow()
{
	m_Running = false;

	delete m_AudioWindow;

	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

int32_t AudioSDLWindow::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		logger::log_error("%s.", SDL_GetError());
		return -1;
	}

	m_Window = SDL_CreateWindow(
		m_WindowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);

	SDL_SetWindowMinimumSize(m_Window, 500, 300);

	return 0;
}

int32_t AudioSDLWindow::CreateContext()
{
	m_glContext = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_glContext);

	SDL_GL_SetSwapInterval(1);

	return 0;
}

int32_t AudioSDLWindow::CreateGlad()
{
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		logger::log_error("Couldn't initialize GLAD.");
		return -1;
	}
	logger::log_info("Initialized GLAD.");

	return 0;
}

int32_t AudioSDLWindow::CreateImGui()
{
	m_AudioWindow = new AudioImGuiWindow(*m_Window, m_AudioSystem);
	m_AudioWindow->CreateImGui(m_glContext, "#version 130");

	return 0;
}

void AudioSDLWindow::RenderWindow()
{
	const ImVec4 background = ImVec4(35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f);

	glViewport(0, 0, windowWidth, windowHeight);

	while (m_Running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glClearColor(background.x, background.y, background.z, background.w);

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
		m_AudioWindow->RenderImGui();

		SDL_GL_SwapWindow(m_Window);
	}
	logger::log_info("Main loop ended.");
}