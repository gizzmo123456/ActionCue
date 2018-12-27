// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SButton.h"

typedef TMulticastDelegate< void, int > FButtonPressed;
DECLARE_MULTICAST_DELEGATE_OneParam( FButtonPressed, int );

/**
 * Base Class for button functionality required for ActionCue
 */
class ACTIONCUE_EDITOR_API BaseButton
{
public:
	/*
	* Initialize BaseButton with a button ID
	* @Param bid:	Button Id.
	*/
	BaseButton( int bid );
	virtual ~BaseButton();
	/** Get the button being displayed */
	TSharedRef<SButton> GetButton();
	/** Get the display size value */
	float GetValue();
	/** Set the display size value */
	void SetValue( float v ); 

	/** has the button been selected for use */
	bool IsSet() const;
	/** set the selected value */
	void Set( bool b );

	/** Checks if the sample id is within the range assigned to this button */
	bool IsSampleInRange( int sampleId );
	/** Assign the sample range for this button */
	void SetSampleRange( int startSanp, int endSamp ); 

	// button pressed callback to send the button ID back to ActionCue_editor
	FButtonPressed OnButtonPressed;

protected:
	TSharedRef<SButton> button = SNew( SButton );

	virtual FSlateColor GetButtonColour() const = 0;	//Button color callback.
	virtual FReply ButtonAction();						//Button Action callback.

private:

	int buttonId = 0;

	float value = 0.0f;			//value range: 0.0f - 1.0f
	bool buttonIsSet = false;

	int startSample = 0;
	int endSample = 1;

	int totalSample = 1;
};
