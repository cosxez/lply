#pragma once

void lply_bsmli()
{
}

void lply_mttit(ma_engine *eng,ma_sound *sound,short range,unsigned long long int mcp,char l_mode=-127)
{
	unsigned long long int ccp;ma_sound_get_cursor_in_pcm_frames(sound,&ccp);

	long long int offset=(long long int)ma_engine_get_sample_rate(eng)*range;
	if (offset>0 && offset+ccp>=mcp){if (l_mode==END_REPLAY){ma_sound_seek_to_pcm_frame(sound,0);if (!ma_sound_is_playing(sound)){ma_sound_start(sound);}}if (l_mode==END_STOP){ma_sound_seek_to_pcm_frame(sound,0);if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}}if (l_mode==END_NEXT){lply_bsmli();}return;}
	if (offset<0 && -offset>ccp){ma_sound_seek_to_pcm_frame(sound,0);return;}
	ma_sound_seek_to_pcm_frame(sound,ccp+offset);
}
