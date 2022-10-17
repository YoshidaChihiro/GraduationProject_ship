#include "Audio.h"
#include<fstream>
#include<cassert>

float Audio::volume_bgm = 1.0f;
float Audio::volume_se = 1.0f;

XAudio2VoiceCallback voiceCallback = {};

ComPtr<IXAudio2> Audio::xAudio2 = {};
IXAudio2MasteringVoice* Audio::masterVoice = {};
std::map<std::string, File> Audio::soundFiles = {};
//std::map<std::string, IXAudio2SourceVoice*> Audio::soundVoices = {};
std::map<std::string, IXAudio2SourceVoice* > Audio::seSourceVoices = {};
std::map<std::string, IXAudio2SourceVoice* > Audio::bgmSourceVoices = {};

Audio::Audio()
{
}

Audio::~Audio()
{
}

void Audio::Initialize()
{
	HRESULT result;
	//XAudioエンジンのインスタンスを作成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを作成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

void Audio::LoadFile(const std::string& keyName, const std::string& fileName)
{
	File waveFile;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//.wavファイルをバイナリモードで開く
	std::string str = "Resources/Sound/" + fileName;
	const char* filename = str.c_str();
	file.open(filename, std::ios_base::binary);

	//ファイルオープン失敗を検出する
	if (file.fail())assert(0);

	//RIFFヘッダーの読み込み
	file.read((char*)&waveFile.riffheader, sizeof(waveFile.riffheader));
	//ファイルがRIFFかチェック
	if (strncmp(waveFile.riffheader.chunk.id.data(), "RIFF", 4) != 0)assert(0);

	//Formatチャンクの読み込み
	file.read((char*)&waveFile.fChunk, sizeof(waveFile.fChunk));
	if (strncmp(waveFile.fChunk.chunk.id.data(), "fmt ", 4) != 0)assert(0);

	//Dataチャンクの読み込み
	file.read((char*)&waveFile.chunk, sizeof(waveFile.chunk));
	if (strncmp(waveFile.chunk.id.data(), "data", 4) != 0)assert(0);

	//Dataチャンクのデータ部(波形データ)の読み込み
	waveFile.buff = new char[waveFile.chunk.size];
	file.read(waveFile.buff, waveFile.chunk.size);

	//Waveファイルを閉じる
	file.close();

	soundFiles.emplace(keyName, waveFile);
}

void Audio::End()
{
	masterVoice->DestroyVoice();
	if (xAudio2.Get() != nullptr)xAudio2->StopEngine();
	for (auto& it : soundFiles)
	{
		delete[]it.second.buff;
	}
}

void Audio::VolumeChangeBGM(const std::string& keyName, const float& soundVol)
{
	if (bgmSourceVoices[keyName] == nullptr)
		return;
	bgmSourceVoices[keyName]->SetVolume(soundVol, XAUDIO2_COMMIT_NOW);
}

void Audio::VolumeChangeSE(const std::string& keyName, const float& soundVol)
{
	if (seSourceVoices[keyName] == nullptr)
		return;
	seSourceVoices[keyName]->SetVolume(soundVol, XAUDIO2_COMMIT_NOW);
}

/*
void Audio::PlayWave(const std::string& keyName, const float& soundVol, bool loop, int loopCount)
{
	HRESULT result;

	//サウンド再生
	WAVEFORMATEX wfex{};

	if (soundVoices[keyName] != nullptr && loop)
		return;

	//波形フォーマットの設定
	memcpy(&wfex, &soundFiles[keyName].fChunk.fmt, sizeof(soundFiles[keyName].fChunk.fmt));
	wfex.wBitsPerSample = soundFiles[keyName].fChunk.fmt.nBlockAlign * 8 / soundFiles[keyName].fChunk.fmt.nChannels;

	//波形フォーマットを元にSourceVoiceの再生
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result)
	{
		delete[] soundFiles[keyName].buff;
		return;
	}
	pSourceVoice->SetVolume(soundVol);
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundFiles[keyName].buff;
	buf.pContext = soundFiles[keyName].buff;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundFiles[keyName].chunk.size;
	if (loop)
	{
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = loopCount;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));
	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
	soundVoices[keyName] = pSourceVoice;
}

void Audio::StopWave(const std::string& keyName)
{
	if (soundVoices[keyName] == nullptr)
		return;
	soundVoices[keyName]->Stop();
	soundVoices[keyName]->DestroyVoice();
	soundVoices.erase(keyName);
}
void Audio::Play(const std::string& arg_name)
{
	soundVoices[arg_name]->Start();
}



void Audio::Stop(const std::string& arg_name)
{
	if (soundVoices[arg_name] == nullptr) return;
	soundVoices[arg_name]->Stop();
}
*/

void Audio::PlayBGM(const std::string& arg_name, const float arg_volume)
{
	HRESULT result;

	//サウンド再生
	WAVEFORMATEX wfex{};

	//波形フォーマットの設定
	memcpy(&wfex, &soundFiles[arg_name].fChunk.fmt, sizeof(soundFiles[arg_name].fChunk.fmt));
	wfex.wBitsPerSample = soundFiles[arg_name].fChunk.fmt.nBlockAlign * 8 / soundFiles[arg_name].fChunk.fmt.nChannels;

	//波形フォーマットを元にSourceVoiceの再生
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result)
	{
		delete[] soundFiles[arg_name].buff;
		return;
	}
	pSourceVoice->SetVolume(arg_volume);
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundFiles[arg_name].buff;
	buf.pContext = soundFiles[arg_name].buff;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundFiles[arg_name].chunk.size;
	buf.LoopBegin = 0;
	buf.LoopLength = 0;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));
	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
	bgmSourceVoices[arg_name] = pSourceVoice;
}

void Audio::StopBGM(const std::string& arg_name)
{
	if (bgmSourceVoices[arg_name] == nullptr)
		return;
	bgmSourceVoices[arg_name]->Stop();
	bgmSourceVoices[arg_name]->DestroyVoice();
	bgmSourceVoices.erase(arg_name);
}

void Audio::PauseBGM(const std::string& arg_name)
{
	if (bgmSourceVoices[arg_name] == nullptr) return;
	bgmSourceVoices[arg_name]->Stop();
}

void Audio::ResumeBGM(const std::string& arg_name)
{
	bgmSourceVoices[arg_name]->Start();
}

void Audio::PlaySE(const std::string& arg_name, const float arg_volume, bool loop, int loopCount)
{
	if (seSourceVoices[arg_name] != nullptr && loop) return;

	HRESULT result;

	//サウンド再生
	WAVEFORMATEX wfex{};

	//波形フォーマットの設定
	memcpy(&wfex, &soundFiles[arg_name].fChunk.fmt, sizeof(soundFiles[arg_name].fChunk.fmt));
	wfex.wBitsPerSample = soundFiles[arg_name].fChunk.fmt.nBlockAlign * 8 / soundFiles[arg_name].fChunk.fmt.nChannels;

	//波形フォーマットを元にSourceVoiceの再生
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result)
	{
		delete[] soundFiles[arg_name].buff;
		return;
	}
	pSourceVoice->SetVolume(arg_volume);
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundFiles[arg_name].buff;
	buf.pContext = soundFiles[arg_name].buff;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundFiles[arg_name].chunk.size;
	if (loop)
	{
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = loopCount;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));
	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
	seSourceVoices[arg_name] = pSourceVoice;
}

void Audio::StopSE(const std::string& arg_name)
{
	if (seSourceVoices[arg_name] == nullptr)
		return;
	seSourceVoices[arg_name]->Stop();
	seSourceVoices[arg_name]->DestroyVoice();
	seSourceVoices.erase(arg_name);
}

void Audio::PauseSE(const std::string& arg_name)
{
	if (seSourceVoices[arg_name] == nullptr) return;
	seSourceVoices[arg_name]->Stop();
}

void Audio::ResumeSE(const std::string& arg_name)
{
	seSourceVoices[arg_name]->Start();
}

void Audio::AllPauseSE()
{
	for (auto itr = seSourceVoices.begin(); itr != seSourceVoices.end(); ++itr)
	{
		if (itr->second == nullptr) continue;
		itr->second->Stop();
	}
}

void Audio::AllResumeSE()
{
	for (auto itr = seSourceVoices.begin(); itr != seSourceVoices.end(); ++itr)
	{
		if (itr->second == nullptr) continue;
		itr->second->Start();
	}
}

void Audio::AllStopSE()
{
	for (auto itr = seSourceVoices.begin(); itr != seSourceVoices.end(); ++itr)
	{
		if (itr->second == nullptr) continue;
		itr->second->Stop();
		itr->second->DestroyVoice();
	}
	seSourceVoices.clear();
}
