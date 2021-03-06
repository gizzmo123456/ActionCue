// Ashley M Sands (gizzmo123456) 2018

#include "AudioUtills.h"
#include "Sound/SoundWave.h"

AudioUtills::AudioUtills()
{

	audioClip = nullptr;

}

AudioUtills::~AudioUtills()
{
}

float AudioUtills::GetAmplitudeData(int startSample, int endSample )
{

	// Check that there is raw sound data to read from.
	if ( !audioClip || audioClip->RawData.GetBulkDataSize() <= 0 )
	{
		UE_LOG( LogTemp, Error, TEXT( "Error: No sample data to read." ) );
		return 0.0f;
	}

	//lock the raw data and get its pointer
	uint8* rawWaveData = ( uint8* )audioClip->RawData.Lock( LOCK_READ_ONLY );
	int32 rawDataSize = audioClip->RawData.GetBulkDataSize();

	FWaveModInfo waveInfo;

	float avgSampleValue = 0.0f;

	//Check that the wave header can be read.
	if ( waveInfo.ReadWaveHeader( rawWaveData, rawDataSize, 0 ) )
	{
		//Take the channels into account
		startSample *= channels;
		endSample *= channels;

		if ( endSample >= (totalSamples * channels) )
			endSample = ( (totalSamples * channels) - 1 );

		int32 sampleRange = endSample - startSample;

		//Get the pointer to the start of the audio samples.
		int16* samplePtr = reinterpret_cast< int16* >( waveInfo.SampleDataStart );

		//apply the startSample offset to the sample ptr
		samplePtr += startSample;

		for ( int i = 0; i < sampleRange; i += channels )
		{
			avgSampleValue += FMath::Abs( *samplePtr );
			samplePtr += channels;
		}

		avgSampleValue /=  ( sampleRange / 2.0f );

	}

	// Unlock the raw data.
	audioClip->RawData.Unlock();

	return avgSampleValue;
}


void AudioUtills::SetAudioClip( USoundWave* clip )
{

	audioClip = clip;

	// Check that there is raw sound data to read from.
	if ( audioClip == nullptr || audioClip->RawData.GetBulkDataSize() <= 0 )
	{
		channels = 0;
		sampleRate = 0;
		totalSamples = 0;

		UE_LOG( LogTemp, Error, TEXT( "Error: No sample data to read." ) );
		return;
	}

	//lock the raw data and get its pointer
	uint8* rawWaveData = ( uint8* )audioClip->RawData.Lock( LOCK_READ_ONLY );
	int32 rawDataSize = audioClip->RawData.GetBulkDataSize();

	FWaveModInfo waveInfo;

	//Check that the wave header can be read.
	if ( waveInfo.ReadWaveHeader( rawWaveData, rawDataSize, 0 ) )
	{
		channels = clip->NumChannels;
		sampleRate = *waveInfo.pSamplesPerSec;
		totalSamples = sampleRate * audioClip->GetDuration();	//This is the total samples for a single channel. GetAmp deals with the channels
	}

	audioClip->RawData.Unlock();

}

float AudioUtills::SamplesToSeconds( int sample )
{
	return (float)sample / (float)sampleRate;
}

int AudioUtills::SecondsToSamples( float time )
{
	return FMath::FloorToInt(time * sampleRate);
}