#pragma once
#include <xaudio2.h>

class AudioCallback : public IXAudio2VoiceCallback
{
public:
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired)override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnStreamEnd() override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override;
	COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error) override;
};

