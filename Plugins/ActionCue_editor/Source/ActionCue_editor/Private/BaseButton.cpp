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
