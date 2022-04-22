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

        static int sensitivity = 0;
        std::string silence_detect_text = "Silence Detection##Silence_Detection_" + std::to_string(a_SoundHash);
        if (ImGui::Button(silence_detect_text.c_str()))
        {
            uaudio::DATA_Chunk data_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::DATA_Chunk>(uaudio::DATA_CHUNK_ID);
            uint32_t pos = a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID);
            for (int32_t i = a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID) - 1; i > -1; i--)
            {
                if (data_chunk.data[i - 1] > sensitivity)
                {
                    pos = i;
                    break;
                }
            }
            a_WaveFile->SetEndPosition(pos);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        std::string silence_detect_sensitivity_text = "Sensitivity##Silence_Detection_Sensitivity_" + std::to_string(a_SoundHash);
        ImGui::InputInt(silence_detect_sensitivity_text.c_str(), &sensitivity, 1);
        ImGui::PopItemWidth();
    }

    const std::string chunk_specific_info_text = "Chunk Specific Info##Chunk_Specific_Info_" + std::to_string(a_SoundHash);
    if (ImGui::CollapsingHeader(chunk_specific_info_text.c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::FMT_CHUNK_ID))
	    {
            const std::string chunk_fmt_text = "FMT##FMT_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_fmt_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::FMT_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::FMT_CHUNK_ID)).c_str());
                ShowValue("Audio Format: ", std::to_string(fmt_chunk.audioFormat).c_str());
                ShowValue("Number of Channels: ", std::to_string(fmt_chunk.numChannels).c_str());
                ShowValue("Sample Rate: ", std::to_string(fmt_chunk.sampleRate).c_str());
                ShowValue("Byte Rate: ", std::to_string(fmt_chunk.byteRate).c_str());
                ShowValue("Block Align: ", std::to_string(fmt_chunk.blockAlign).c_str());
                ShowValue("Bits per Sample: ", std::to_string(fmt_chunk.bitsPerSample).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::DATA_CHUNK_ID))
	    {
            const std::string chunk_data_text = "DATA##DATA_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_data_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::DATA_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::DATA_CHUNK_ID)).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::ACID_CHUNK_ID))
	    {
            const std::string chunk_acid_text = "ACID##ACID_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_acid_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::ACID_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::ACID_CHUNK_ID)).c_str());

                uaudio::ACID_Chunk acid_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::ACID_Chunk>(uaudio::ACID_CHUNK_ID);
                char type_of_file[256] = {};
                snprintf(type_of_file, 256, "0x%x", acid_chunk.type_of_file);
                ShowValue("Type of File: ", std::string(type_of_file).c_str());
                char root_note[256] = {};
                snprintf(root_note, 256, "0x%x", acid_chunk.root_note);
                ShowValue("Root Note: ", std::string(root_note).c_str());
                char unknown1[256] = {};
                snprintf(unknown1, 256, "0x%x", acid_chunk.unknown1);
                ShowValue("Unknown 1: ", std::string(unknown1).c_str());
                ShowValue("Unknown 2: ", std::to_string(acid_chunk.unknown2).c_str());
                ShowValue("Number of Beats: ", std::to_string(acid_chunk.num_of_beats).c_str());
                ShowValue("Meter Denominator: ", std::to_string(acid_chunk.meter_denominator).c_str());
                ShowValue("Meter Numerator: ", std::to_string(acid_chunk.meter_numerator).c_str());
                ShowValue("Tempo: ", std::to_string(acid_chunk.tempo).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::BEXT_CHUNK_ID))
	    {
            const std::string chunk_bext_text = "BEXT##BEXT_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_bext_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::BEXT_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::BEXT_CHUNK_ID)).c_str());

                uaudio::BEXT_Chunk bext_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::BEXT_Chunk>(uaudio::BEXT_CHUNK_ID);
                ShowValue("Description: ", std::string(bext_chunk.description).c_str());
                ShowValue("Originator: ", std::string(bext_chunk.originator).c_str());
                ShowValue("Originator Reference: ", std::string(bext_chunk.originator_reference).c_str());
                ShowValue("Originator Date: ", std::string(bext_chunk.origination_date).c_str());
                ShowValue("Originator Time: ", std::string(bext_chunk.origination_time).c_str());
                ShowValue("Time Reference Low: ", std::to_string(bext_chunk.time_reference_low).c_str());
                ShowValue("Time Reference High: ", std::to_string(bext_chunk.time_reference_high).c_str());
                ShowValue("Version: ", std::to_string(bext_chunk.version).c_str());
                ShowValue("UMID: ", std::string(reinterpret_cast<const char*>(bext_chunk.umid)).c_str());
                ShowValue("Loudness Value: ", std::to_string(bext_chunk.loudness_value).c_str());
                ShowValue("Loudness Range: ", std::to_string(bext_chunk.loudness_range).c_str());
                ShowValue("Max True Peak Level: ", std::to_string(bext_chunk.max_true_peak_level).c_str());
                ShowValue("Max Momentary Loudness: ", std::to_string(bext_chunk.max_momentary_loudness).c_str());
                ShowValue("Max Short Term Loudness: ", std::to_string(bext_chunk.max_short_term_loudness).c_str());
                ShowValue("Reserved: ", std::string(reinterpret_cast<const char*>(bext_chunk.reserved)).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::FACT_CHUNK_ID))
	    {
            const std::string chunk_fact_text = "FACT##FACT_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_fact_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::FACT_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::FACT_CHUNK_ID)).c_str());

                uaudio::FACT_Chunk fact_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::FACT_Chunk>(uaudio::FACT_CHUNK_ID);
                ShowValue("Sample Length: ", std::to_string(fact_chunk.sample_length).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::CUE_CHUNK_ID))
	    {
            const std::string chunk_cue_text = "CUE##CUE_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_cue_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::CUE_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::CUE_CHUNK_ID)).c_str());

                uaudio::CUE_Chunk cue_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::CUE_Chunk>(uaudio::CUE_CHUNK_ID);
                ShowValue("Number of cue points: ", std::to_string(cue_chunk.num_cue_points).c_str());
                for (uint32_t i = 0; i < cue_chunk.num_cue_points; i++)
                {
                    const std::string chunk_cue_point_text = "CUE " + std::to_string(i) + "##CUE_" + std::to_string(a_SoundHash) + "_CUE_POINT_" + std::to_string(i);
                    if (ImGui::CollapsingHeader(chunk_cue_point_text.c_str()))
                    {
                        ImGui::Indent(IMGUI_INDENT);
                        ShowValue("ID: ", std::to_string(cue_chunk.cue_points[i].id).c_str());
                        ShowValue("Position: ", std::to_string(cue_chunk.cue_points[i].position).c_str());
                        ShowValue("Data Chunk ID: ", std::string(reinterpret_cast<const char*>(cue_chunk.cue_points[i].data_chunk_id)).c_str());
                        ShowValue("Chunk Start: ", std::to_string(cue_chunk.cue_points[i].chunk_start).c_str());
                        ShowValue("Block Start: ", std::to_string(cue_chunk.cue_points[i].block_start).c_str());
                        ShowValue("Block Offset: ", std::to_string(cue_chunk.cue_points[i].sample_offset).c_str());
                        ImGui::Unindent(IMGUI_INDENT);
                    }
                }
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::SMPL_CHUNK_ID))
	    {
            const std::string chunk_smpl_text = "SMPL##SMPL_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_smpl_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::SMPL_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::SMPL_CHUNK_ID)).c_str());

                uaudio::SMPL_Chunk smpl_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::SMPL_Chunk>(uaudio::SMPL_CHUNK_ID);
                ShowValue("Manufacturer: ", std::to_string(smpl_chunk.manufacturer).c_str());
                ShowValue("Product: ", std::to_string(smpl_chunk.product).c_str());
                ShowValue("Sample Period: ", std::to_string(smpl_chunk.sample_period).c_str());
                ShowValue("MIDI Unity Node: ", std::to_string(smpl_chunk.midi_unity_node).c_str());
                ShowValue("MIDI Pitch Fraction: ", std::to_string(smpl_chunk.midi_pitch_fraction).c_str());
                ShowValue("SMPTE Format: ", std::to_string(smpl_chunk.smpte_format).c_str());
                ShowValue("SMPTE Offset: ", std::to_string(smpl_chunk.smpte_offset).c_str());
                ShowValue("Number of Sample loops: ", std::to_string(smpl_chunk.num_sample_loops).c_str());
                ShowValue("Sampler Data: ", std::to_string(smpl_chunk.sampler_data).c_str());
                for (uint32_t i = 0; i < smpl_chunk.num_sample_loops; i++)
                {
                    const std::string chunk_smpl_loop_text = "LOOP " + std::to_string(i) + "##SMPL_" + std::to_string(a_SoundHash) + "_LOOP_" + std::to_string(i);
                    if (ImGui::CollapsingHeader(chunk_smpl_loop_text.c_str()))
                    {
                        ImGui::Indent(IMGUI_INDENT);
                        ShowValue("Cue Point ID: ", std::to_string(smpl_chunk.samples[i].cue_point_id).c_str());
                        ShowValue("Type: ", std::to_string(smpl_chunk.samples[i].type).c_str());
                        ShowValue("Start: ", std::to_string(smpl_chunk.samples[i].start).c_str());
                        ShowValue("Start in time: ", std::string(
                            uaudio::WaveFile::FormatDuration(uaudio::utils::PosToSeconds(smpl_chunk.samples[i].start * 2, fmt_chunk.byteRate), true))
                            .c_str());
                        ShowValue("End: ", std::to_string(smpl_chunk.samples[i].end).c_str());
                        ShowValue("End in time: ", std::string(
                            uaudio::WaveFile::FormatDuration(uaudio::utils::PosToSeconds(smpl_chunk.samples[i].end * 2, fmt_chunk.byteRate), true))
                            .c_str());
                        ShowValue("Fraction: ", std::to_string(smpl_chunk.samples[i].fraction).c_str());
                        ShowValue("Play Count: ", std::to_string(smpl_chunk.samples[i].play_count).c_str());

                        const std::string chunk_smpl_loop_button_start_text = "Set As Start Position##Start_Position_" + std::to_string(i) + "_SMPL_" + std::to_string(a_SoundHash);
                        if (ImGui::Button(chunk_smpl_loop_button_start_text.c_str()))
                            a_WaveFile->SetStartPosition(smpl_chunk.samples[i].start * 2);

                        const std::string chunk_smpl_loop_button_end_text = "Set As End Position##End_Position_" + std::to_string(i) + "_SMPL_" + std::to_string(a_SoundHash);
                        if (ImGui::Button(chunk_smpl_loop_button_end_text.c_str()))
                            a_WaveFile->SetEndPosition(smpl_chunk.samples[i].end * 2);

                        ImGui::Unindent(IMGUI_INDENT);
                    }
                }
                ImGui::Unindent(IMGUI_INDENT);
            }
	    }
	    if (a_WaveFile->GetWaveFormat().HasChunk(uaudio::TLST_CHUNK_ID))
	    {
            const std::string chunk_smpl_text = "TLST##TLST_" + std::to_string(a_SoundHash);
            if (ImGui::CollapsingHeader(chunk_smpl_text.c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Chunk ID: ", uaudio::TLST_CHUNK_ID);
                ShowValue("Chunk Size: ", std::to_string(a_WaveFile->GetWaveFormat().GetChunkSize(uaudio::TLST_CHUNK_ID)).c_str());

                uaudio::TLST_Chunk tlst_chunk = a_WaveFile->GetWaveFormat().GetChunkFromData<uaudio::TLST_Chunk>(uaudio::TLST_CHUNK_ID);
                ShowValue("List: ", std::to_string(tlst_chunk.list).c_str());
                ShowValue("Name: ", std::string(reinterpret_cast<const char*>(tlst_chunk.name)).c_str());
                ShowValue("Type: ", std::to_string(tlst_chunk.type).c_str());
                ShowValue("Trigger on 1: ", std::to_string(tlst_chunk.trigger_on_1).c_str());
                ShowValue("Trigger on 2: ", std::to_string(tlst_chunk.trigger_on_2).c_str());
                ShowValue("Trigger on 3: ", std::to_string(tlst_chunk.trigger_on_3).c_str());
                ShowValue("Trigger on 4: ", std::to_string(tlst_chunk.trigger_on_4).c_str());
                ShowValue("Func: ", std::to_string(tlst_chunk.func).c_str());
                ShowValue("Extra: ", std::to_string(tlst_chunk.extra).c_str());
                ShowValue("Extra Data: ", std::to_string(tlst_chunk.extra_data).c_str());
                ImGui::Unindent(IMGUI_INDENT);
            }
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