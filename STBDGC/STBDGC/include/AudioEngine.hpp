#ifndef AUDIO_ENGINE
#define AUDIO_ENGINE

//OpenAl
#include <al.h>
#include <alc.h>
//libsndfile
#include <sndfile.h>

#include <map>
class AudioEngine
{
public:
	AudioEngine();
	ALuint loadSndFile(const std::string& Filename);
	void playBGM(const std::string path, const float volume = 0.5f);
	void stopBGM();
private:
	ALCdevice* Device;
	ALCcontext* Context;
    ALuint bgmBuffer; 
	ALuint bgm;
	bool locked;
	std::map<std::string, ALuint> buffers;


};

#endif
