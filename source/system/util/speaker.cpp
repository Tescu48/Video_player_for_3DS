#include "system/headers.hpp"

bool util_speaker_init = false;
ndspWaveBuf util_ndsp_buffer[24][DEF_SPEAKER_MAX_BUFFERS];

Result_with_string Util_speaker_init(void)
{
	Result_with_string result;
	if(util_speaker_init)
		goto already_inited;

	result.code = ndspInit();//0xd880A7FA
	if(result.code != 0)
	{
		result.error_description = "[Error] ndspInit() failed. ";
		goto nintendo_api_failed;
	}
	util_speaker_init = true;
	return result;

	already_inited:
	result.code = DEF_ERR_ALREADY_INITIALIZED;
	result.string = DEF_ERR_ALREADY_INITIALIZED_STR;
	return result;

	nintendo_api_failed:
	result.string = DEF_ERR_NINTENDO_RETURNED_NOT_SUCCESS_STR;
	return result;
}

Result_with_string Util_speaker_set_audio_info(int play_ch, int music_ch, int sample_rate)
{
	float mix[12] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	Result_with_string result;
	if(!util_speaker_init)
		goto not_inited;
	
	if(play_ch < 0 || play_ch > 23 || music_ch <= 0 || music_ch > 2 || sample_rate <= 0)
		goto invalid_arg;
	
	ndspChnReset(play_ch);
	ndspChnWaveBufClear(play_ch);
	ndspChnSetMix(play_ch, mix);
	if(music_ch == 2)
	{
		ndspChnSetFormat(play_ch, NDSP_FORMAT_STEREO_PCM16);
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	}
	else
	{
		ndspChnSetFormat(play_ch, NDSP_FORMAT_MONO_PCM16);
		ndspSetOutputMode(NDSP_OUTPUT_MONO);
	}
	
	ndspChnSetInterp(play_ch, NDSP_INTERP_LINEAR);
	ndspChnSetRate(play_ch, sample_rate);
	for(int i = 0; i < DEF_SPEAKER_MAX_BUFFERS; i++)
	{
		Util_safe_linear_free((void*)util_ndsp_buffer[play_ch][i].data_vaddr);
		util_ndsp_buffer[play_ch][i].data_vaddr = NULL;
		memset(util_ndsp_buffer[play_ch], 0, sizeof(util_ndsp_buffer[play_ch]));
	}

	return result;

	not_inited:
	result.code = DEF_ERR_NOT_INITIALIZED;
	result.string = DEF_ERR_NOT_INITIALIZED_STR;
	return result;

	invalid_arg:
	result.code = DEF_ERR_INVALID_ARG;
	result.string = DEF_ERR_INVALID_ARG_STR;
	return result;
}

Result_with_string Util_speaker_add_buffer(int play_ch, int music_ch, u8* buffer, int size)
{
	int free_queue = -1;
	Result_with_string result;
	if(!util_speaker_init)
		goto not_inited;
	
	if(play_ch < 0 || play_ch > 23 || music_ch <= 0 || music_ch > 2 || !buffer || size <= 0)
		goto invalid_arg;

	//Search for free queue
	for(int i = 0; i < DEF_SPEAKER_MAX_BUFFERS; i++)
	{
		if(util_ndsp_buffer[play_ch][i].status == NDSP_WBUF_FREE || util_ndsp_buffer[play_ch][i].status == NDSP_WBUF_DONE)
		{
			free_queue = i;
			break;
		}
	}

	if(free_queue == -1)
	{
		result.error_description = "[Error] Queues are full. ";
		goto try_again;
	}

	Util_safe_linear_free((void*)util_ndsp_buffer[play_ch][free_queue].data_vaddr);
	util_ndsp_buffer[play_ch][free_queue].data_vaddr = NULL;
	util_ndsp_buffer[play_ch][free_queue].data_vaddr = (u8*)Util_safe_linear_alloc(size);
	if(!util_ndsp_buffer[play_ch][free_queue].data_vaddr)
		goto out_of_linear_memory;

	memcpy((void*)util_ndsp_buffer[play_ch][free_queue].data_vaddr, buffer, size);

	util_ndsp_buffer[play_ch][free_queue].nsamples = size / (2 * music_ch);
	ndspChnWaveBufAdd(play_ch, &util_ndsp_buffer[play_ch][free_queue]);

	return result;

	not_inited:
	result.code = DEF_ERR_NOT_INITIALIZED;
	result.string = DEF_ERR_NOT_INITIALIZED_STR;
	return result;

	invalid_arg:
	result.code = DEF_ERR_INVALID_ARG;
	result.string = DEF_ERR_INVALID_ARG_STR;
	return result;

	try_again:
	result.code = DEF_ERR_TRY_AGAIN;
	result.string = DEF_ERR_TRY_AGAIN_STR;
	return result;

	out_of_linear_memory:
	result.code = DEF_ERR_OUT_OF_LINEAR_MEMORY;
	result.string = DEF_ERR_OUT_OF_LINEAR_MEMORY_STR;
	return result;
}

void Util_speaker_clear_buffer(int play_ch)
{
	if(!util_speaker_init)
		return;
	if(play_ch < 0 || play_ch > 23)
		return;
	
	ndspChnWaveBufClear(play_ch);
	for(int i = 0; i < DEF_SPEAKER_MAX_BUFFERS; i++)
	{
		Util_safe_linear_free((void*)util_ndsp_buffer[play_ch][i].data_vaddr);
		util_ndsp_buffer[play_ch][i].data_vaddr = NULL;
	}
}

void Util_speaker_pause(int play_ch)
{
	if(!util_speaker_init)
		return;
	if(play_ch < 0 || play_ch > 23)
		return;

	ndspChnSetPaused(play_ch, true);
}

void Util_speaker_resume(int play_ch)
{
	if(!util_speaker_init)
		return;
	if(play_ch < 0 || play_ch > 23)
		return;

	ndspChnSetPaused(play_ch, false);
}

bool Util_speaker_is_paused(int play_ch)
{
	if(!util_speaker_init)
		return false;
	else if(play_ch < 0 || play_ch > 23)
		return false;
	else
		return ndspChnIsPaused(play_ch);
}

bool Util_speaker_is_playing(int play_ch)
{
	if(!util_speaker_init)
		return false;
	else if(play_ch < 0 || play_ch > 23)
		return false;
	else
		return ndspChnIsPlaying(play_ch);
}

void Util_speaker_exit(void)
{
	if(!util_speaker_init)
		return;
	
	for(int i = 0; i < 24; i++)
		Util_speaker_clear_buffer(i);

	util_speaker_init = false;
	ndspExit();
}
