#pragma once
#include <array>
#include <cstdint>
#include <wrl.h>
#include <xaudio2.h>
#include <string>
#include <map>

#include"XAudio2VoiceCallback.h"

#pragma comment(lib,"xaudio2.lib")

using namespace Microsoft::WRL;

//チャンクヘッダ
struct Chunk
{
	std::array<char, 4> id;;//チャンクごとのID
	int32_t size;//チャンクサイズ
};

//RIFFヘッダチャンク
struct RiffHeader
{
	Chunk chunk;//"RIFF"
	std::array<char, 4> type;//"WAVE"
};

//FMTチャンク
struct FormatChunk
{
	Chunk chunk;//"fmt"
	WAVEFORMAT fmt;//波形フォーマット
};

struct File
{
	Chunk chunk;
	RiffHeader riffheader;
	FormatChunk fChunk;
	char* buff;
};

class Audio
{
public:
	Audio();
	~Audio();
	static void Initialize();
	static void LoadFile(const std::string& keyName, const std::string& fileName);
	static void End();	

	static void VolumeChangeBGM(const std::string& keyName, const float& soundVol);
	static void VolumeChangeSE(const std::string& keyName, const float& soundVol);

	//再生・停止
	static void PlayBGM(const std::string& arg_name, const float arg_volume);
	static void StopBGM(const std::string& arg_name);
	//一時停止・再開
	static void PauseBGM(const std::string& arg_name);
	static void ResumeBGM(const std::string& arg_name);

	static void PlaySE(const std::string& arg_name, const float arg_volume, bool loop = false, int loopCount = XAUDIO2_LOOP_INFINITE);
	static void StopSE(const std::string& arg_name);
	
	static void PauseSE(const std::string& arg_name);
	static void ResumeSE(const std::string& arg_name);

	static void AllPauseSE();
	static void AllResumeSE();

	static void AllStopSE();

	static float volume_bgm;
	static float volume_se;

private:
	static ComPtr<IXAudio2> xAudio2;
	static IXAudio2MasteringVoice* masterVoice;
	static std::map <std::string, File> soundFiles;
	//static std::map <std::string, IXAudio2SourceVoice*> soundVoices;
	static std::map <std::string, IXAudio2SourceVoice*> bgmSourceVoices;
	static std::map <std::string, IXAudio2SourceVoice*> seSourceVoices;
};