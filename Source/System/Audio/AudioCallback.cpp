#include "AudioCallback.h"
#include "../Common/Logger.h"

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnVoiceProcessingPassStart(UINT32 BytesRequired)
{
	// 処理パスが始まったことを示すデバッグメッセージ
	LOG("OnVoiceProcessingPassStart\n");
	LOG("%d\n", BytesRequired);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnVoiceProcessingPassEnd()
{
	// 処理パスが終了したことを示すデバッグメッセージ
	LOG("OnVoiceProcessingPassEnd\n");
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnStreamEnd()
{
	// ストリーム再生が終了したことを示すデバッグメッセージ
	LOG("OnStreamEnd\n");
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnBufferStart(void* pBufferContext)
{
	// 新しいバッファ再生が始まったことを示すデバッグメッセージ
	LOG("OnBufferStart\n");
	LOG("%s\n", pBufferContext);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnBufferEnd(void* pBufferContext)
{
	// バッファ再生が終了したことを示すデバッグメッセージ
	LOG("OnBufferEnd\n");
	LOG("%s\n", pBufferContext);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnLoopEnd(void* pBufferContext)
{
	// ループ再生が終了したことを示すデバッグメッセージ
	LOG("OnLoopEnd\n");
	LOG("%s\n", pBufferContext);
}

COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE AudioCallback::OnVoiceError(void* pBufferContext, HRESULT Error)
{
	// エラーが発生したことを示すデバッグメッセージ
	LOG("OnVoiceError\n");
	LOG("ErrorCode:%n\n", Error);
	LOG("%s\n", pBufferContext);
}
