#include "tools/SoundsTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "utils/Utils.h"
#include "wave/WaveChunks.h"
#include "wave/WaveReader.h"

SoundsTool::SoundsTool(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem) : BaseTool(0, "Sounds", "Sounds"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
    m_Channel = uaudio::xaudio2::XAudio2Channel(m_AudioSystem);
}

void SoundsTool::Render()
{
    for (const auto hash : m_SoundSystem.GetSoundHashes())
        RenderSound(hash);
}

void SoundsTool::RenderSound(UAUDIO_DEFAULT_HASH a_SoundHash)
{
    uaudio::WaveFile *a_WaveFile = m_SoundSystem.FindSound(a_SoundHash);
    ImGui::Text("%s", a_WaveFile->GetSoundTitle());

    std::string play_button_text = std::string(PLAY) + " Play##Play_Sound_" + a_WaveFile->GetSoundTitle();
    if (ImGui::Button(play_button_text.c_str()))
        m_AudioSystem.Play(*a_WaveFile);

    ImGui::SameLine();
    std::string remove_sound_text = std::string(MINUS) + " Unload##Unload_Sound_" + a_WaveFile->GetSoundTitle();
    if (ImGui::Button(remove_sound_text.c_str()))
    {
        for (uint32_t i = 0; i < m_AudioSystem.ChannelSize(); i++)
        {
            uaudio::xaudio2::XAudio2Channel *channel = m_AudioSystem.GetChannel(i);
            if (&channel->GetSound() == a_WaveFile)
            {
                channel->RemoveSound();
                channel->ResetPos();
                channel->Stop();
            }
        }
        m_SoundSystem.UnloadSound(a_SoundHash);
        return;
    }

    ImGui::SameLine();
    std::string save_sound_text = std::string(SAVE) + " Save##Save_Sound_" + a_WaveFile->GetSoundTitle();
    if (ImGui::Button(save_sound_text.c_str()))
        SaveFile(a_WaveFile);

    ImGui::SameLine();
    bool isLooping = a_WaveFile->IsLooping();
    std::string loop_button_text = std::string(RETRY) + "##Loop_Sound_" + std::to_string(a_SoundHash);
    if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
        a_WaveFile->SetLooping(isLooping);

    const uaudio::FMT_Chunk fmt_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::FMT_Chunk>(uaudio::FMT_CHUNK_ID);

    const std::string loop_options_text = "Loop Options##Loop_Options_Sound_" + std::to_string(a_SoundHash);
    if (ImGui::CollapsingHeader(loop_options_text.c_str()))
    {
        // TODO: Int range slider.
        float start_position = static_cast<float>(a_WaveFile->GetStartPosition());
        float end_position = static_cast<float>(a_WaveFile->GetEndPosition());
        ImGui::Text("%s", std::string(
                              uaudio::WaveFile::FormatDuration(uaudio::utils::PosToSeconds(a_WaveFile->GetStartPosition(), fmt_chunk.byteRate), false) +
                              "/" +
                              uaudio::WaveFile::FormatDuration(uaudio::utils::PosToSeconds(a_WaveFile->GetEndPosition(), fmt_chunk.byteRate), false))
                              .c_str());
        const uint32_t start_position_temp = static_cast<uint32_t>(start_position), end_position_temp = static_cast<uint32_t>(end_position);
        std::string range_slider_sound_name_text = "##Range_Slider_Sound_" + std::to_string(a_SoundHash);
        if (ImGui::RangeSliderFloat(range_slider_sound_name_text.c_str(), &start_position, &end_position, 0.0f, static_cast<float>(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID)), ""))
        {
            const uint32_t start_position_int = static_cast<uint32_t>(start_position);
            const uint32_t end_position_int = static_cast<uint32_t>(end_position);
            m_Channel.SetSound(*a_WaveFile);
            if (start_position_temp != start_position_int)
            {
                uint32_t new_start_position = start_position_int % static_cast<int>(m_AudioSystem.GetBufferSize());
                new_start_position = start_position_int - new_start_position;
                new_start_position = uaudio::utils::clamp<uint32_t>(new_start_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
                m_Channel.PlayRanged(new_start_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
                a_WaveFile->SetStartPosition(new_start_position);
            }
            else if (end_position_temp != end_position_int)
            {
                uint32_t new_end_position = end_position_int % static_cast<int>(m_AudioSystem.GetBufferSize());
                new_end_position = end_position_int - new_end_position;
                new_end_position = uaudio::utils::clamp<uint32_t>(new_end_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
                a_WaveFile->SetEndPosition(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
                m_Channel.PlayRanged(new_end_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
                a_WaveFile->SetEndPosition(new_end_position);
            }
        }

        const uint32_t start_position_int = static_cast<uint32_t>(start_position);
        std::string left_button_start_text = std::string(LEFT) + "##Left_Sound_Start_" + std::to_string(a_SoundHash);
        if (ImGui::Button(left_button_start_text.c_str(), ImVec2(25, 25)))
        {
            int32_t new_start_position = start_position_int - static_cast<int>(m_AudioSystem.GetBufferSize());
            new_start_position = uaudio::utils::clamp<int32_t>(new_start_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
            m_Channel.SetSound(*a_WaveFile);
            m_Channel.PlayRanged(new_start_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
            a_WaveFile->SetStartPosition(new_start_position);
        }

        ImGui::SameLine();
        std::string right_button_start_text = std::string(RIGHT) + "##Right_Sound_Start_" + std::to_string(a_SoundHash);
        if (ImGui::Button(right_button_start_text.c_str(), ImVec2(25, 25)))
        {
            uint32_t new_start_position = start_position_int + static_cast<int>(m_AudioSystem.GetBufferSize());
            new_start_position = uaudio::utils::clamp<uint32_t>(new_start_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
            m_Channel.SetSound(*a_WaveFile);
            m_Channel.PlayRanged(new_start_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
            a_WaveFile->SetStartPosition(new_start_position);
        }

        const uint32_t end_position_int = static_cast<uint32_t>(end_position);
        std::string left_button_end_text = std::string(LEFT) + "##Left_Sound_End_" + std::to_string(a_SoundHash);
        if (ImGui::Button(left_button_end_text.c_str(), ImVec2(25, 25)))
        {
            int32_t new_end_position = end_position_int - static_cast<int>(m_AudioSystem.GetBufferSize());
            new_end_position = uaudio::utils::clamp<int32_t>(new_end_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
            m_Channel.SetSound(*a_WaveFile);
            m_Channel.PlayRanged(new_end_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
            a_WaveFile->SetEndPosition(new_end_position);
        }

        ImGui::SameLine();
        std::string right_button_end_text = std::string(RIGHT) + "##Right_Sound_End_" + std::to_string(a_SoundHash);
        if (ImGui::Button(right_button_end_text.c_str(), ImVec2(25, 25)))
        {
            uint32_t new_end_position = end_position_int + static_cast<int>(m_AudioSystem.GetBufferSize());
            new_end_position = uaudio::utils::clamp<uint32_t>(new_end_position, 0, a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
            m_Channel.SetSound(*a_WaveFile);
            a_WaveFile->SetEndPosition(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID));
            m_Channel.PlayRanged(new_end_position, static_cast<int>(m_AudioSystem.GetBufferSize()));
            a_WaveFile->SetEndPosition(new_end_position);
        }

        if (ImGui::Button("Automatically Detect Silence"))
        {
            uaudio::DATA_Chunk data_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::DATA_Chunk>(uaudio::DATA_CHUNK_ID);
            uint32_t pos = a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID);
            for (int32_t i = a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID) - 1; i > -1; i--)
            {
                if (data_chunk.data[i - 1] != 0)
                {
                    pos = i;
                    break;
                }
            }
            a_WaveFile->SetEndPosition(pos);
        }
    }

    const std::string extra_info_text = "Extra Info##ExtraInfo_Sound_" + std::to_string(a_SoundHash);
    if (ImGui::CollapsingHeader(extra_info_text.c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);

        if (ImGui::CollapsingHeader(std::string(std::string("Deeper File Info###DeeperFileInfo_") + a_WaveFile->GetSoundTitle()).c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);
            ShowValue("Block Align: ", std::string(std::to_string(fmt_chunk.blockAlign)).c_str());
            ShowValue("Chunk Size: ", std::string(std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID))).c_str());
            ImGui::Unindent(IMGUI_INDENT);
        }
        ShowValue("Name: ", a_WaveFile->GetSoundTitle());
        ShowValue("Hash: ", std::to_string(a_SoundHash).c_str());
        ShowValue("Format: ", std::to_string(fmt_chunk.audioFormat).c_str());
        ShowValue("Number Of Channels: ", std::string(std::to_string(fmt_chunk.numChannels) + (fmt_chunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
        ShowValue("Sample Rate: ", std::string(std::to_string(fmt_chunk.sampleRate) + " Hz").c_str());
        ShowValue("Byte Rate: ", std::to_string(fmt_chunk.byteRate).c_str());
        ShowValue("Bits Per Sample: ", std::to_string(fmt_chunk.bitsPerSample).c_str());
        ShowValue("Duration (sec): ", std::string(std::to_string(uaudio::WaveFile::GetDuration(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID), fmt_chunk.byteRate)) + " secs").c_str());
        ShowValue("Duration: ", uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID), fmt_chunk.byteRate)).c_str());
        if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::BEXT_CHUNK_ID))
        {
            const uaudio::BEXT_Chunk bext_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::BEXT_Chunk>(uaudio::BEXT_CHUNK_ID);
            ShowValue("Origination Date: ", std::string(&bext_chunk.origination_date[0], &bext_chunk.origination_date[0] + std::size(bext_chunk.origination_date)).c_str());
            ShowValue("Origination Time: ", std::string(&bext_chunk.origination_time[0], &bext_chunk.origination_time[0] + std::size(bext_chunk.origination_time)).c_str());
        }
        if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::ACID_CHUNK_ID))
        {
            const uaudio::ACID_Chunk acid_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::ACID_Chunk>(uaudio::ACID_CHUNK_ID);
            ShowValue("Tempo: ", std::to_string(acid_chunk.tempo).c_str());
        }

        ImGui::Unindent(IMGUI_INDENT);
    }
    ImGui::Separator();
}

/// <summary>
/// Opens a wav file and adds it to the resources.
/// </summary>
void SoundsTool::SaveFile(const uaudio::WaveFile *a_WaveFile)
{
    OPENFILENAME ofn;
    TCHAR sz_file[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = sz_file;
    ofn.nMaxFile = sizeof(sz_file);
    ofn.lpstrFilter = L"WAV Files (*.wav;*.wave)\0*.wav;*.wave";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        const auto path = new char[wcslen(ofn.lpstrFile) + 1];
        wsprintfA(path, "%S", ofn.lpstrFile);

        uaudio::WaveReader::SaveSound(path, a_WaveFile->GetWaveFormat());
        delete[] path;
    }
}