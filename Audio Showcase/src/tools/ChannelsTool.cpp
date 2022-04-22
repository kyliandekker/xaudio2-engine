#include "tools/ChannelsTool.h"

#include <uaudio/wave/high_level/WaveFile.h>

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include <uaudio/utils/Utils.h>
#include <uaudio/wave/high_level/WaveChunks.h>

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
    if (!a_Channel->IsInUse())
        return;

    uaudio::FMT_Chunk fmt_chunk = a_Channel->GetSound().GetWaveFormat().GetChunkFromData<uaudio::FMT_Chunk>(uaudio::FMT_CHUNK_ID);

    bool active = a_Channel->GetActive();
    std::string on_off_button_text = "##OnOff_Channel_" + std::to_string(a_Index);
    if (ImGui::OnOffButton(on_off_button_text.c_str(), &active, ImVec2(25, 25)))
        a_Channel->SetActive(active);

    ImGui::SameLine();
    float panning = a_Channel->GetPanning();
    std::string panning_tooltip_text = std::string(PANNING) + " Panning (affects channel " + std::to_string(a_Index) + ")";
    std::string panning_text = "##Panning_Channel_" + std::to_string(a_Index);
    if (ImGui::Knob(panning_text.c_str(), &panning, -1, 1, ImVec2(25, 25), panning_tooltip_text.c_str(), 0.0f))
        a_Channel->SetPanning(panning);

    ImGui::SameLine();
    float volume = a_Channel->GetVolume();
    std::string volume_tooltip_text = std::string(VOLUME_UP) + " Volume (affects channel " + std::to_string(a_Index) + ")";
    std::string volume_text = "##Volume_Channel_" + std::to_string(a_Index);
    if (ImGui::Knob(volume_text.c_str(), &volume, 0, 1, ImVec2(25, 25), volume_tooltip_text.c_str(), 1.0f))
        a_Channel->SetVolume(volume);

    int32_t pos = static_cast<int32_t>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS));
    float final_pos = uaudio::utils::PosToSeconds(a_Channel->GetSound().GetEndPosition(), fmt_chunk.byteRate);
    ImGui::Text("%s", std::string(
							uaudio::utils::FormatDuration(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_S), false) +
							"/" +
							uaudio::utils::FormatDuration(final_pos, false))
							.c_str());
    uint32_t final_pos_slider = a_Channel->IsInUse() ? a_Channel->GetSound().GetEndPosition() : 5000;
    std::string player_text = std::string("###Player_" + std::to_string(a_Index));
    if (ImGui::SliderInt(player_text.c_str(), &pos, 0, static_cast<int>(final_pos_slider), ""))
    {
        uint32_t new_pos = pos % static_cast<int>(m_AudioSystem.GetBufferSize());
        uint32_t final_new_pos = pos - new_pos;
        final_new_pos = uaudio::utils::clamp<uint32_t>(final_new_pos, 0, a_Channel->GetSound().GetEndPosition());
        a_Channel->SetPos(final_new_pos);
        if (!a_Channel->IsPlaying())
            a_Channel->PlayRanged(final_new_pos, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    if (a_Channel->IsPlaying())
    {
        std::string pause_button_text = std::string(PAUSE) + "##Pause_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(pause_button_text.c_str(), ImVec2(25, 25)))
            a_Channel->Pause();
    }
    else
    {
        std::string play_button_text = std::string(PLAY) + "##Play_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(play_button_text.c_str(), ImVec2(25, 25)))
            a_Channel->Play();
    }

    ImGui::SameLine();
    std::string left_button_text = std::string(LEFT) + "##Left_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(left_button_text.c_str(), ImVec2(25, 25)))
    {
        int32_t prev_pos = pos - static_cast<int>(m_AudioSystem.GetBufferSize());
        prev_pos = uaudio::utils::clamp<int32_t>(prev_pos, 0, a_Channel->GetSound().GetEndPosition());
        a_Channel->SetPos(prev_pos);
        a_Channel->Pause();
        a_Channel->PlayRanged(prev_pos, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    ImGui::SameLine();
    std::string stop_button_text = std::string(STOP) + "##Stop_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(stop_button_text.c_str(), ImVec2(25, 25)))
    {
        a_Channel->SetPos(0);
        a_Channel->Pause();
    }

    ImGui::SameLine();
    std::string right_button_text = std::string(RIGHT) + "##Right_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(right_button_text.c_str(), ImVec2(25, 25)))
    {
        uint32_t next_pos = pos + static_cast<int>(m_AudioSystem.GetBufferSize());
        next_pos = uaudio::utils::clamp<uint32_t>(next_pos, 0, a_Channel->GetSound().GetEndPosition());
        a_Channel->SetPos(next_pos);
        a_Channel->Pause();
        a_Channel->PlayRanged(next_pos, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    ImGui::SameLine();
    bool isLooping = a_Channel->IsLooping();
    std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
    if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
        a_Channel->SetLooping(isLooping);

    if (a_Channel->IsInUse())
    {
        std::string channel_name_text = "Channel " + std::to_string(a_Index) + " (" + std::string(a_Channel->GetSound().GetWaveFormat().m_FilePath) + ")" + "##Channel_" + std::to_string(a_Index);
        if (ImGui::CollapsingHeader(channel_name_text.c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);
            ShowValue("Currently playing: ", a_Channel->GetSound().GetWaveFormat().m_FilePath.c_str());
            ShowValue("Progress: ", std::string(
                                        uaudio::utils::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(fmt_chunk.byteRate)) +
                                        "/" +
										uaudio::utils::FormatDuration(uaudio::utils::GetDuration(a_Channel->GetSound().GetEndPosition(), fmt_chunk.byteRate)))
                                        .c_str());
            ShowValue("Time Left: ", std::string(
										uaudio::utils::FormatDuration(uaudio::utils::GetDuration(a_Channel->GetSound().GetEndPosition(), fmt_chunk.byteRate) - (static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(fmt_chunk.byteRate))))
                                         .c_str());
            ShowValue("Progress (position): ", std::string(
                                                   std::to_string(static_cast<int>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS))) +
                                                   "/" +
                                                   std::to_string(a_Channel->GetSound().GetEndPosition()))
                                                   .c_str());
            ImGui::Unindent(IMGUI_INDENT);
        }
    }
    ImGui::Separator();
}