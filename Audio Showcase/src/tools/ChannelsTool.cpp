#include "tools/ChannelsTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>

ChannelsTool::ChannelsTool(uaudio::AudioSystem &a_AudioSystem) : BaseTool(0, "Channels", "Channels"), m_AudioSystem(a_AudioSystem)
{
}

void ChannelsTool::Render()
{
    for (uint32_t i = 0; i < m_AudioSystem.ChannelSize(); i++)
        RenderChannel(i, m_AudioSystem.GetChannel(uaudio::ChannelHandle(i)));
}

void ChannelsTool::RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel *a_Channel)
{
    if (ImGui::CollapsingHeader(std::string("Channel " + std::to_string(a_Index) + "##Channel_" + std::to_string(a_Index)).c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);
        ShowValue("Is in use: ", a_Channel->IsInUse() ? "true" : "false");

        if (a_Channel->IsInUse())
        {
            if (ImGui::CollapsingHeader(std::string("Channel Info###ChannelInfo_" + std::to_string(a_Index)).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);

                ShowValue("Currently playing: ", a_Channel->GetSound().GetSoundTitle());
                ShowValue("Progress", std::string(
                                          uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                                          "/" +
                                          uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                                          .c_str());
                ShowValue("Time Left", std::string(
                                           uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate) - (static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate))))
                                           .c_str());

                ImGui::Unindent(IMGUI_INDENT);
            }

            if (ImGui::CollapsingHeader(std::string("Channel Actions###ChannelActions_" + std::to_string(a_Index)).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);

                std::string panning_text = std::string(PANNING) + " Panning";
                ImGui::Text("%s", panning_text.c_str());
                ImGui::SameLine();
                float panning = a_Channel->GetPanning();
                ImGui::SliderFloat(std::string("###Panning_Channel_" + std::to_string(a_Index)).c_str(), &panning, -1, 1);
                a_Channel->SetPanning(panning);

                std::string volume_text = std::string(VOLUME_UP) + " Volume";
                ImGui::Text("%s", volume_text.c_str());
                ImGui::SameLine();
                float volume = a_Channel->GetVolume();
                ImGui::SliderFloat(std::string("###Volume_Channel_" + std::to_string(a_Index)).c_str(), &volume, 0, 1);
                a_Channel->SetVolume(volume);

                int32_t pos = static_cast<int32_t>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS));
                ImGui::Text("%s", std::string(
                                      uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                                      "/" +
                                      uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                                      .c_str());
                if (ImGui::SliderInt(std::string("###Player_" + std::to_string(a_Index)).c_str(), &pos, 0.0f, a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, ""))
                {
                    uint32_t newtest = pos % static_cast<int>(m_AudioSystem.GetBufferSize());
                    uint32_t finaltest = pos - newtest;
                    a_Channel->SetPos(finaltest);
                    if (!a_Channel->IsPlaying())
                        a_Channel->PlayRanged(finaltest, static_cast<int>(m_AudioSystem.GetBufferSize()));
                }
                if (a_Channel->IsPlaying())
                {
                    std::string pause_button = std::string(PAUSE) + " Pause##Pause_Sound_" + std::to_string(a_Index);
                    if (ImGui::Button(pause_button.c_str()))
                        a_Channel->Pause();
                }
                else
                {
                    std::string play_button = std::string(PLAY) + " Play##Play_Sound_" + std::to_string(a_Index);
                    if (ImGui::Button(play_button.c_str()))
                        a_Channel->Play();
                }

                ImGui::Unindent(IMGUI_INDENT);
            }
        }
        ImGui::Unindent(IMGUI_INDENT);
    }
}