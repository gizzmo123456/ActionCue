// Ashley M Sands (gizzmo123456) 2018

#include "BaseButton.h"

BaseButton::BaseButton( int bid )
{
	buttonId = bid;

	value = 0.0f;

	button = SNew( SButton )
		.OnClicked_Raw( this, &BaseButton::ButtonAction )
		.ButtonColorAndOpacity_Raw( this, &BaseButton::GetButtonColour );
}

BaseButton::~BaseButton(){}

TSharedRef<SButton> BaseButton::GetButton()
{
	return button;
}

float BaseButton::GetValue()
{
	return value;
}

void BaseButton::SetValue(float v)
{
	value = v;
}

bool BaseButton::IsSet() const
{
	return buttonIsSet;
}

void BaseButton::Set( bool b )
{
	buttonIsSet = b;
}

int BaseButton::GetSample( SampleRangeType rangeType )
{
	switch ( rangeType )
	{
		case SampleRangeType::Start:
			return startSample;
		case SampleRangeType::End:
			return endSample;
		default:
			return 0;
	}
}

int BaseButton::GetAvgSample()
{
	return FMath::FloorToInt( (startSample + endSample) / 2.0f );
}

bool BaseButton::IsSampleInRange( int sampId )
{
	return sampId >= startSample && sampId <= endSample;
}

void BaseButton::SetSampleRange( int startSanp, int endSamp )
{
	startSample = startSanp;
	endSample = endSamp;
}

FReply BaseButton::ButtonAction()
{
	buttonIsSet = !buttonIsSet;

	if( OnButtonPressed.IsBound() )
		OnButtonPressed.Broadcast( buttonId );

	return FReply::Handled();
}
