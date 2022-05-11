#include "tools/ChannelsTool.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include <uaudio/utils/Utils.h>
#include <uaudio/wave/high_level/WaveChunks.h>

#include <uaudio/system/high_level/Channel.h>
#include <uaudio/system/high_level/SoundSystem.h>
#include <uaudio/wave/low_level/WaveFormat.h>
#include <uaudio/utils/Utils.h>

ChannelsTool::ChannelsTool(uaudio::AudioSystem &a_AudioSystem) : BaseTool(0, "Channels", "Channels"), m_AudioSystem(a_AudioSystem)
{
}

void ChannelsTool::Render()
{
    size_t size = 0;
    m_AudioSystem.GetComponentsSize(size);
    for (uint32_t i = 0; i < size; i++)
        RenderChannel(i, m_AudioSystem.GetComponent(i));
}

void ChannelsTool::RenderChannel(uint32_t a_Index, uaudio::AudioComponent& a_Component)
{
    bool isActive = false;
    a_Component.IsActive(isActive);
    if (!isActive)
        return;

    uaudio::WaveFormat* waveFormat = nullptr;
    uaudio::UAUDIO_DEFAULT_HASH hash = 0;
    a_Component.GetHash(hash);
    uaudio::SoundSys.FindSound(waveFormat, hash);

    uaudio::FMT_Chunk fmt_chunk;
	waveFormat->GetChunkFromData<uaudio::FMT_Chunk>(fmt_chunk, uaudio::FMT_CHUNK_ID);

    bool playback = false;
    a_Component.GetPlayback(playback);
    std::string on_off_button_text = "##OnOff_Channel_" + std::to_string(a_Index);
    if (ImGui::OnOffButton(on_off_button_text.c_str(), &playback, ImVec2(25, 25)))
        a_Component.SetPlayback(playback);

    ImGui::SameLine();
    float panning;
	a_Component.GetPanning(panning);
    std::string panning_tooltip_text = std::string(PANNING) + " Panning (affects channel " + std::to_string(a_Index) + ")";
    std::string panning_text = "##Panning_Channel_" + std::to_string(a_Index);
    if (ImGui::Knob(panning_text.c_str(), &panning, -1, 1, ImVec2(25, 25), panning_tooltip_text.c_str(), 0.0f))
        a_Component.SetPanning(panning);

    ImGui::SameLine();
    float volume;
    a_Component.GetVolume(volume);
    std::string volume_tooltip_text = std::string(VOLUME_UP) + " Volume (affects channel " + std::to_string(a_Index) + ")";
    std::string volume_text = "##Volume_Channel_" + std::to_string(a_Index);
    if (ImGui::Knob(volume_text.c_str(), &volume, 0, 1, ImVec2(25, 25), volume_tooltip_text.c_str(), 1.0f))
        a_Component.SetVolume(volume);

    uint32_t pos = 0;
    uaudio::Channel* channel = nullptr;
    int32_t channel_index = -1;

    if (channel_index == -1)
        return;

    a_Component.GetChannelIndex(channel_index);
    m_AudioSystem.GetSoundChannel(channel, channel_index);
    channel->GetPosition(pos, uaudio::TIMEUNIT::TIMEUNIT_POS);

    int end_pos = 0;
    end_pos = static_cast<int>(pos);
    float final_pos = uaudio::utils::PosToSeconds(pos, fmt_chunk.byteRate);

    uint32_t final_pos_slider = 0;
    waveFormat->GetEndPosition(final_pos_slider);

    ImGui::Text("%s", std::string(
                          uaudio::utils::FormatDuration(uaudio::utils::PosToSeconds(pos, fmt_chunk.byteRate), false) +
                          "/" +
                          uaudio::utils::FormatDuration(uaudio::utils::PosToSeconds(final_pos_slider, fmt_chunk.byteRate), false))
                          .c_str());

    uaudio::BUFFERSIZE buffer_size;
    m_AudioSystem.GetBufferSize(buffer_size);

    std::string player_text = std::string("###Player_" + std::to_string(a_Index));
    if (ImGui::SliderInt(player_text.c_str(), &end_pos, 0, static_cast<int>(final_pos_slider), ""))
    {
        int32_t new_pos = end_pos % static_cast<int>(buffer_size);
        uint32_t final_new_pos = end_pos - new_pos;
        final_new_pos = uaudio::utils::clamp<uint32_t>(final_new_pos, 0, end_pos);
        channel->SetPosition(final_new_pos);
		channel->PlayRanged(final_new_pos, static_cast<uint32_t>(buffer_size));
    }

    bool isPlaying = false;
    a_Component.IsPlaying(isPlaying);
    if (isPlaying)
    {
        std::string pause_button_text = std::string(PAUSE) + "##Pause_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(pause_button_text.c_str(), ImVec2(25, 25)))
            a_Component.Pause();
    }
    else
    {
        std::string play_button_text = std::string(PLAY) + "##Play_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(play_button_text.c_str(), ImVec2(25, 25)))
            a_Component.Resume();
    }

    ImGui::SameLine();
    std::string left_button_text = std::string(LEFT) + "##Left_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(left_button_text.c_str(), ImVec2(25, 25)))
    {
        uint32_t prev_pos = pos - static_cast<uint32_t>(buffer_size);
        prev_pos = uaudio::utils::clamp<uint32_t>(prev_pos, 0, final_pos_slider);
        channel->SetPosition(prev_pos);
        channel->Pause();
        channel->PlayRanged(prev_pos, static_cast<uint32_t>(buffer_size));
    }

    ImGui::SameLine();
    std::string stop_button_text = std::string(STOP) + "##Stop_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(stop_button_text.c_str(), ImVec2(25, 25)))
    {
        channel->SetPosition(0);
        channel->Pause();
    }

    ImGui::SameLine();
    std::string right_button_text = std::string(RIGHT) + "##Right_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(right_button_text.c_str(), ImVec2(25, 25)))
    {
        uint32_t next_pos = pos + static_cast<uint32_t>(buffer_size);
        next_pos = uaudio::utils::clamp<uint32_t>(next_pos, 0, final_pos_slider);
        channel->SetPosition(next_pos);
        channel->Pause();
        channel->PlayRanged(next_pos, static_cast<uint32_t>(buffer_size));
    }

    ImGui::SameLine();
    bool isLooping = false;
	a_Component.IsLooping(isLooping);
    std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
    if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
        a_Component.SetLooping(isLooping);

    std::string channel_name_text = "Channel " + std::to_string(a_Index) + " (" + std::string(waveFormat->m_FilePath) + ")" + "##Channel_" + std::to_string(a_Index);
    if (ImGui::CollapsingHeader(channel_name_text.c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);
        ShowValue("Currently playing: ", waveFormat->m_FilePath.c_str());
        ShowValue("Progress: ", std::string(
            uaudio::utils::FormatDuration(static_cast<float>(pos) / static_cast<float>(fmt_chunk.byteRate)) +
            "/" +
            uaudio::utils::FormatDuration(uaudio::utils::GetDuration(final_pos_slider, fmt_chunk.byteRate)))
            .c_str());
        ShowValue("Time Left: ", std::string(
            uaudio::utils::FormatDuration(uaudio::utils::GetDuration(final_pos_slider, fmt_chunk.byteRate) - (static_cast<float>(pos) / static_cast<float>(fmt_chunk.byteRate))))
            .c_str());
        ShowValue("Progress (position): ", std::string(
            std::to_string(static_cast<int>(final_pos_slider)) +
            "/" +
            std::to_string(final_pos_slider))
            .c_str());
        ImGui::Unindent(IMGUI_INDENT);
    }
    ImGui::Separator();
}