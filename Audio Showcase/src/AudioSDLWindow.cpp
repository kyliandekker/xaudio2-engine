#include "AudioSDLWindow.h"

#include <iostream>

#include <uaudio/utils/Logger.h>
#include <glad/glad.h>
#include <imgui/backends/imgui_impl_opengl3.h>

AudioSDLWindow::AudioSDLWindow(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem) : m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
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

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

ImVec2 AudioSDLWindow::GetWindowSize() const
{
	int sdl_width = 0, sdl_height = 0;
	SDL_GetWindowSize(m_Window, &sdl_width, &sdl_height);
	return ImVec2(static_cast<float>(sdl_width), static_cast<float>(sdl_height));
}

int32_t AudioSDLWindow::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		uaudio::logger::ASSERT(false, "%s.", SDL_GetError());
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
		uaudio::logger::ASSERT(false, "Couldn't initialize GLAD.");
		return -1;
	}
	uaudio::logger::log_info("Initialized GLAD.");

	return 0;
}

int32_t AudioSDLWindow::CreateImGui()
{
	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// setup platform/renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(m_Window, m_glContext);
	ImGui_ImplOpenGL3_Init("#version 130");
	m_AudioWindow = new AudioImGuiWindow(this, &m_AudioSystem, &m_SoundSystem);
	m_AudioWindow->CreateImGui();

	return 0;
}

void AudioSDLWindow::RenderWindow()
{
	const ImVec4 background = ImVec4(15 / 255.0f, 15 / 255.0f, 15 / 255.0f, 1.00f);

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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_Window);
		m_AudioWindow->Render();
		m_AudioSystem.UpdateNonExtraThread();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(m_Window);
	}
	uaudio::logger::log_info("Main loop ended.");
}

AudioImGuiWindow &AudioSDLWindow::GetImGuiWindow() const
{
	return *m_AudioWindow;
}