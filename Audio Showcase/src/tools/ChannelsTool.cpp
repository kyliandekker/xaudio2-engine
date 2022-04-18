#include "tools/ChannelsTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

ChannelsTool::ChannelsTool(uaudio::AudioSystem& a_AudioSystem) : BaseTool(0, "Channels", "Channels"), m_AudioSystem(a_AudioSystem)
{
}

void ChannelsTool::Render()
{
    for (uint32_t i = 0; i < m_AudioSystem.ChannelSize(); i++)
        RenderChannel(i, m_AudioSystem.GetChannel(uaudio::ChannelHandle(i)));
}

void ChannelsTool::RenderChannel(uint32_t a_Index, uaudio::xaudio2::XAudio2Channel* a_Channel)
{
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
    float current_pos = static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->IsInUse() ? a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate : 1.0f);
    float final_pos = a_Channel->IsInUse() ? uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate) : 0.0f;
    ImGui::Text("%s", std::string(
        uaudio::WaveFile::FormatDuration(current_pos, false) +
        "/" +
        uaudio::WaveFile::FormatDuration(final_pos, false))
        .c_str());
    int final_pos_slider = a_Channel->IsInUse() ? a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize : 5000;
    if (ImGui::SliderInt(std::string("###Player_" + std::to_string(a_Index)).c_str(), &pos, 0.0f, final_pos_slider, ""))
    {
        uint32_t newtest = pos % static_cast<int>(m_AudioSystem.GetBufferSize());
        uint32_t finaltest = pos - newtest;
        a_Channel->SetPos(finaltest);
        if (!a_Channel->IsPlaying())
            a_Channel->PlayRanged(finaltest, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    if (a_Channel->IsPlaying())
    {
        std::string pause_button = std::string(PAUSE) + "##Pause_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(pause_button.c_str(), ImVec2(25, 25)))
            a_Channel->Pause();
    }
    else
    {
        std::string play_button = std::string(PLAY) + "##Play_Sound_" + std::to_string(a_Index);
        if (ImGui::Button(play_button.c_str(), ImVec2(25, 25)))
            a_Channel->Play();
    }

    ImGui::SameLine();
    std::string left_button = std::string(LEFT) + "##Left_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(left_button.c_str(), ImVec2(25, 25)))
    {
        uint32_t prev_pos = pos + static_cast<int>(m_AudioSystem.GetBufferSize());
        a_Channel->SetPos(prev_pos);
        a_Channel->Pause();
        a_Channel->PlayRanged(prev_pos, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    ImGui::SameLine();
    std::string stop_button = std::string(STOP) + "##Stop_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(stop_button.c_str(), ImVec2(25, 25)))
    {
        a_Channel->SetPos(0);
        a_Channel->Pause();
    }

    ImGui::SameLine();
    std::string right_button = std::string(RIGHT) + "##Right_Sound_" + std::to_string(a_Index);
    if (ImGui::Button(right_button.c_str(), ImVec2(25, 25)))
    {
        uint32_t next_pos = pos + static_cast<int>(m_AudioSystem.GetBufferSize());
        a_Channel->SetPos(next_pos);
        a_Channel->Pause();
        a_Channel->PlayRanged(next_pos, static_cast<int>(m_AudioSystem.GetBufferSize()));
    }

    ImGui::SameLine();
    bool isLooping = a_Channel->IsLooping();
    std::string loop_button = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
    if (ImGui::CheckboxButton(loop_button.c_str(), ImVec2(25, 25), &isLooping))
        a_Channel->SetLooping(isLooping);

    if (a_Channel->IsInUse())
    {
        std::string channel_name_text = "Channel " + std::to_string(a_Index) + " (" + std::string(a_Channel->GetSound().GetSoundTitle()) + ")" + "##Channel_" + std::to_string(a_Index);
        if (ImGui::CollapsingHeader(channel_name_text.c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);
            ShowValue("Currently playing: ", a_Channel->GetSound().GetSoundTitle());
            ShowValue("Progress: ", std::string(
                uaudio::WaveFile::FormatDuration(static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)) +
                "/" +
                uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate)))
                .c_str());
            ShowValue("Time Left: ", std::string(
                uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize, a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate) - (static_cast<float>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS)) / static_cast<float>(a_Channel->GetSound().GetWavFormat().fmtChunk.byteRate))))
                .c_str());
            ShowValue("Progress (position): ", std::string(
                std::to_string(static_cast<int>(a_Channel->GetPos(uaudio::TIMEUNIT::TIMEUNIT_POS))) +
                "/" +
                std::to_string(a_Channel->GetSound().GetWavFormat().dataChunk.chunkSize))
                .c_str());
            ImGui::Unindent(IMGUI_INDENT);
        }
    }
    ImGui::Separator();
}