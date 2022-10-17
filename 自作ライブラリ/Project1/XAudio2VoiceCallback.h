#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
class XAudio2VoiceCallback :public IXAudio2VoiceCallback
{
public:
	//ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
	//バッファストリームの再生が終了したとき
	STDMETHOD_(void, OnStreamEnd)(THIS) {};
	//バッファぼ使用開始時
	STDMETHOD_(void, OnBufferStart)(THIS_ void* pBufferContext) {};
	//バッファぼ末尾に達した時
	STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext) {
		//バッファを開放する
		delete[] pBufferContext;
	};
	//再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd)(THIS_ void* pBufferContext) {};
	//ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError)(THIS_ void* pBufferContext, HRESULT Error) {};

};
