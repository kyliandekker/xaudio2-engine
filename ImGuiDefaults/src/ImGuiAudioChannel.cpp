#include "ImGuiAudioChannel.h"

#include <string>
#include <imgui/imgui.h>
#include <xaudio2_engine/wav/WaveFile.h>

ImGuiAudioChannel::ImGuiAudioChannel(AudioSystem& a_AudioSystem)
{
	m_Channel = new XAudio2Channel(a_AudioSystem);
}

ImGuiAudioChannel::ImGuiAudioChannel(XAudio2Channel& a_Channel)
{
    m_Channel = &a_Channel;
}

void ImGuiAudioChannel::RenderElement(uint32_t a_Index)
{
    if (m_Channel == nullptr)
        return;

    ShowValue("Is in use: ", m_Channel->IsInUse() ? "true" : "false");

    if (m_Channel->IsInUse())
    {
        if (ImGui::CollapsingHeader(std::string("Channel Info###ChannelInfo_" + std::to_string(a_Index)).c_str()))
        {
            ImGui::Indent(16.0f);

            ShowValue("Currently playing: ", m_Channel->GetSound().GetSoundTitle());
            ShowValue("Progress", std::string(
                WaveFile::FormatDuration(static_cast<float>(m_Channel->GetPos()) / static_cast<float>(m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                "/" +
                WaveFile::FormatDuration(WaveFile::GetDuration(m_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                .c_str());
            ShowValue("Time Left", std::string(
                WaveFile::FormatDuration(WaveFile::GetDuration(m_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate) - (static_cast<float>(m_Channel->GetPos()) / static_cast<float>(m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate))))
                .c_str());

            ImGui::Unindent(16.0f);
        }

        if (ImGui::CollapsingHeader(std::string("Channel Actions###ChannelActions_" + std::to_string(a_Index)).c_str()))
        {
            ImGui::Indent(16.0f);

            ImGui::Text("Panning");
            ImGui::SameLine();
            float panning = m_Channel->GetPanning();
            ImGui::SliderFloat(std::string("###Panning_Channel_" + std::to_string(a_Index)).c_str(), &panning, -1, 1);
            m_Channel->SetPanning(panning);

            ImGui::Text("Volume");
            ImGui::SameLine();
            float volume = m_Channel->GetVolume();
            ImGui::SliderFloat(std::string("###Volume_Channel_" + std::to_string(a_Index)).c_str(), &volume, 0, 1);
            m_Channel->SetVolume(volume);

            int32_t test = m_Channel->GetPos();
            if (ImGui::SliderInt(std::string("###Player_" + std::to_string(a_Index)).c_str(), &test, 0.0f, m_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, ""))
            {
                uint32_t newtest = test % 8184;
                uint32_t finaltest = test - newtest;
                m_Channel->Pause();
                m_Channel->SetPos(finaltest);
                m_Channel->PlayRanged(finaltest, 8184);
            }
            ShowValue("Current Pos: ", std::string(
                WaveFile::FormatDuration(static_cast<float>(m_Channel->GetPos()) / static_cast<float>(m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                "/" +
                WaveFile::FormatDuration(WaveFile::GetDuration(m_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, m_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                .c_str());
            if (m_Channel->IsPlaying())
            {
                if (ImGui::Button(std::string("Pause###Pause_Channel_" + std::to_string(a_Index)).c_str()))
                    m_Channel->Pause();
            }
            else
            {
                if (ImGui::Button(std::string("Resume###Resume_Channel_" + std::to_string(a_Index)).c_str()))
                    m_Channel->Resume();
            }

            ImGui::Unindent(16.0f);
        }
    }
}

void ImGuiAudioChannel::Update()
{
    if (m_Channel == nullptr)
        return;

    m_Channel->Update();
}
