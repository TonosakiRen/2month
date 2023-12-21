#include "Audio.h"

#include <cassert>

#pragma comment(lib,"xaudio2.lib")

Audio* Audio::GetInstance() {
    static Audio instans;
    return &instans;
}

Audio::~Audio() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        DestroyPlayHandle(i);
    }

    if (masterVoice_) {
        masterVoice_->DestroyVoice();
        masterVoice_ = nullptr;
    }
}

void Audio::Initialize() {
    HRESULT result;
    // XAudioエンジンのインスタンスを作成
    result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));
    // マスターボイスを作成
    result = xAudio2_->CreateMasteringVoice(&masterVoice_);
    assert(SUCCEEDED(result));

    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        DestroyPlayHandle(i);
    }
}

void Audio::Update() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        if (sourceVoices_[i]) {
            XAUDIO2_VOICE_STATE state{ };
            sourceVoices_[i]->GetState(&state);
            if (state.BuffersQueued == 0) {
                DestroyPlayHandle(i);
            }
        }
    }
}

size_t Audio::SoundPlayWave(size_t soundHandle) {
    HRESULT result;
    const SoundData& soundData = soundData_.at(soundHandle);

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer.data();
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    size_t playHandle = FindUnusedPlayHandle();
    // プレイハンドルがいっぱい
    assert(playHandle < kMaxNumPlayHandles);

    // SourceVoice の作成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));

    sourceVoices_[playHandle] = pSourceVoice;
    return playHandle;
}

size_t Audio::SoundPlayLoopStart(size_t soundHandle) {
    HRESULT result;
    const SoundData& soundData = soundData_.at(soundHandle);

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer.data();
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;

    size_t playHandle = FindUnusedPlayHandle();
    // プレイハンドルがいっぱい
    assert(playHandle < kMaxNumPlayHandles);

    // SourceVoice の作成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    result = pSourceVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));

    result = pSourceVoice->Start();
    assert(SUCCEEDED(result));

    sourceVoices_[playHandle] = pSourceVoice;
    return playHandle;
}

void Audio::SoundPlayLoopEnd(size_t playHandle) {
    // soundHandle に対応する SourceVoice を取得
    if (IsValidPlayHandle(playHandle)) {
        sourceVoices_[playHandle]->Stop();
        DestroyPlayHandle(playHandle);
    }
}

size_t Audio::SoundLoadWave(const char* filename) {
    auto iter = std::find_if(soundData_.begin(), soundData_.end(), [&](const SoundData& soundData) {
        return soundData.filename == filename;
        });
    if (iter != soundData_.end()) {
        return std::distance(soundData_.begin(), iter);
    }

#pragma region ファイルオープン
    //directryPass
    std::string directryPass = "Resources/Sounds/";
    // ファイル入出ストリームのインスタンス
    std::ifstream file;
    // wavファイルをバイナリモードで開く
    file.open(directryPass + filename, std::ios_base::binary);
    // ファイルオープン失敗を検出する
    assert(file.is_open());
#pragma endregion
#pragma region wavデータ読み込み
    // RIFFヘッターの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    // ファイルがRIFFがチェック
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
        assert(0);
    }
    // タイプがWAVEがチェック
    if (strncmp(riff.type, "WAVE", 4) != 0) {
        assert(0);
    }
    // Formatチャンクの読み込み
    FormatChunk format = {};
    // チャンクヘッターの確認
    file.read((char*)&format, sizeof(ChunkHearder));
    if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
        assert(0);
    }
    // チャンク本体の読み込み
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);
    // Dataチャンクの読み込み
    ChunkHearder data;
    file.read((char*)&data, sizeof(data));
    // JUNKチャンクを検出した場合
    if (strncmp(data.id, "JUNK", 4) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    // LISTチャンクを検出した場合
    if (strncmp(data.id, "LIST", 4) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    // bextチャンクを検出した場合
    if (strncmp(data.id, "bext", 4) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    // INFOチャンクを検出した場合
    if (strncmp(data.id, "INFO", 4) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    // REAPERチャンクを検出した場合
    if (strncmp(data.id, "REAPER", 6) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    if (strncmp(data.id, "junk", 4) == 0) {
        // 読み取りチャンクを検出した場合
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }
    if (strncmp(data.id, "data", 4) != 0) {
        assert(0);
    }
    // Dataチャンクのデータ部（波形データ）の読み込み
    std::vector<BYTE> pBuffer(data.size);
    file.read(reinterpret_cast<char*>(pBuffer.data()), data.size);

    // Waveファイルを閉じる
    file.close();
#pragma endregion
#pragma region 読み込んだ音声データのreturn
    // returnする為の音声データ
    SoundData soundData = {};
    soundData.filename = filename;
    soundData.wfex = format.fmt;
    soundData.pBuffer = std::move(pBuffer);
    soundData.bufferSize = data.size;
#pragma endregion
    soundData_.emplace_back(soundData);

    return soundData_.size() - 1;
}

void Audio::StopSound(size_t playHandle) {
    assert(playHandle < kMaxNumPlayHandles);
    DestroyPlayHandle(playHandle);
}

void Audio::SetPitch(size_t playHandle, float pitch) {
    assert(playHandle < kMaxNumPlayHandles);
    sourceVoices_[playHandle]->SetFrequencyRatio(pitch);
}

void Audio::SetValume(size_t playHandle, float volume) {
    assert(playHandle < kMaxNumPlayHandles);
    sourceVoices_[playHandle]->SetVolume(volume);
}

bool Audio::IsValidPlayHandle(size_t playHandle) {
    return playHandle < kMaxNumPlayHandles && sourceVoices_[playHandle] != nullptr;
}


size_t Audio::FindUnusedPlayHandle() {
    for (size_t i = 0; i < kMaxNumPlayHandles; ++i) {
        if (sourceVoices_[i] == nullptr) {
            return i;
        }
    }
    return size_t(-1);
}

void Audio::DestroyPlayHandle(size_t playHandle) {
    assert(playHandle < kMaxNumPlayHandles);
    if (sourceVoices_[playHandle]) {
        sourceVoices_[playHandle]->DestroyVoice();
        sourceVoices_[playHandle] = nullptr;
    }
}


void Audio::SoundUnload(size_t soundHandle) {
    // バッファのメモリを解放
    soundData_.at(soundHandle).pBuffer.clear();
    soundData_.at(soundHandle).bufferSize = 0;
    soundData_.at(soundHandle).wfex = {};
    soundData_.erase(soundData_.begin() + soundHandle);
}
