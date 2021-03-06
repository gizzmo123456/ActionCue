// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"

class USoundWave;

/** Some useful audio utility's
 * 
 */
class ACTIONCUE_EDITOR_API AudioUtills
{
public:
	AudioUtills();
	~AudioUtills();

	/** Gets the average amplitude data over a period of time.
		All inputed data is expected to be for a single channel only (mono).
		This will handle the channel offset internally. 

		@Param startSample:			Start of the sample range	
		@Param endSample:			End of the sample range		
	*/
	float GetAmplitudeData(int startSample, int endSample);
	
	void SetAudioClip(USoundWave* clip);

	// Time converter functions
	float SamplesToSeconds( int samp );
	int SecondsToSamples( float time );

	int totalSamples = 0;
	int channels = 0;
	int sampleRate = 0;

private:

	USoundWave* audioClip;

};
