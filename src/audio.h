
#ifndef AUDIO_H
#define AUDIO_H

int InitAudio();
void DeinitAudio();

int LoadMusic(int track);

void PlayMusic();

void MusicVolumeUp();
void MusicVolumeDown();
void EffectVolumeUp();
void EffectVolumeDown();

#endif /* AUDIO_H */
