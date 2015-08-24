#include "pch.hpp"
#include "AudioEngine.hpp"
#include <OgreLogManager.h>

AudioEngine::AudioEngine()
{

	 Device = alcOpenDevice(NULL);
   // if (!Device)
     //   return false;
   	Context = alcCreateContext(Device, NULL);
    //if (!Context)
      //  return false;
     if (!alcMakeContextCurrent(Context))
        Ogre::LogManager::getSingleton().logMessage("Cannot use openal context");
    //return true;


	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);

	ALfloat Orientation[] = {0.0f, 0.0f, 1.0f,
							0.0f, 1.0f, 0.0f};

	alListenerfv(AL_ORIENTATION, Orientation);
	
	alGenSources(1, &bgm);

	locked = false;
}

ALuint AudioEngine::loadSndFile(const std::string& Filename)
{
	
	
	auto query = buffers.find(Filename);
	if(query != buffers.end())
	{
		return query->second;
	}


	// Open Audio file with libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
	{
        return 0;
	}

	//get the number of sample and the samplerate (in samples by seconds)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

	
	//Read samples in 16bits signed
	std::vector<float> SamplesFloat(NbSamples);
    if (sf_read_float(File, &SamplesFloat[0], NbSamples) < NbSamples)
	{
        //return 0;
	}

	std::vector<ALshort> Samples(NbSamples);
	ALshort max = 0x7FFF; //biggest 16bits siged float (positive)
	for(size_t i(0); i < Samples.size(); i++)
		//This will step down a bit the amplitude of the signal to prevent saturation while using some formats
		Samples[i] = max*SamplesFloat[i]*0.88f;

	//close file
    sf_close(File);

	//Read the number of chanels. sound effects should be mono and background music should be stereo
    ALenum Format;
    switch (FileInfos.channels)
    {
		case 1 :
			Format = AL_FORMAT_MONO16;   break;
		case 2 : 
			Format = AL_FORMAT_STEREO16; break;
		
        default : return 0;
    }
	
    //create OpenAL buffer
	ALuint buffer;
    alGenBuffers(1, &buffer);

	// load buffer
    alBufferData(buffer, Format, &Samples[0], NbSamples * sizeof(ALshort), SampleRate);
 
    //check errors
    if (alGetError() != AL_NO_ERROR)
	{
        return 0;
	}

	buffers[Filename] = buffer;
    return buffer;
}

void AudioEngine::playBGM(const std::string path, const float volume)
{
    bgmBuffer = loadSndFile(path);

	alSourcei(bgm, AL_BUFFER, bgmBuffer);	
	alSourcei(bgm, AL_LOOPING, AL_TRUE);
	alSourcef(bgm, AL_GAIN, volume);

	alSourcePlay(bgm);
}

void AudioEngine::stopBGM()
{
	alSourceStop(bgm);
}
