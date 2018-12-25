// Ashley M Sands (gizzmo123456) 2018

#include "AudioUtills.h"
#include "Sound/SoundWave.h"

AudioUtills::AudioUtills()
{
}

AudioUtills::~AudioUtills()
{
}

float AudioUtills::GetAmplitudeData(int startSample, int endSample )
{

	// Check that there is raw sound data to read from.
	if ( audioClip->RawData.GetBulkDataSize() <= 0 )
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

		if ( endSample >= totalSamples )
			endSample = ( totalSamples - 1 );

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

		avgSampleValue /= ( sampleRange / 2.0f );

	}

	// Unlock the raw data.
	audioClip->RawData.Unlock();

	return avgSampleValue;
}


void AudioUtills::SetAudioClip( USoundWave* clip )
{

	audioClip = clip;
	channels = clip->NumChannels;

	// Check that there is raw sound data to read from.
	if ( audioClip->RawData.GetBulkDataSize() <= 0 )
	{
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
		sampleRate = *waveInfo.pSamplesPerSec;
		totalSamples = sampleRate * audioClip->GetDuration();
	}

	audioClip->RawData.Unlock();

}