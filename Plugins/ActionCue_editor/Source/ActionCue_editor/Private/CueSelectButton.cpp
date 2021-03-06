// Ashley M Sands (gizzmo123456) 2018

#include "CueSelectButton.h"

CueSelectButton::CueSelectButton(int bid):BaseButton(bid)
{
	SetValue( 0 );
}

CueSelectButton::~CueSelectButton()
{
}

FSlateColor CueSelectButton::GetButtonColour() const
{
	FLinearColor col = IsSet() ? FLinearColor( 0.5f, 1.0f, 0.0f ) : FLinearColor( 0.0f, 1.0f, 0.5f );
	return col;
}

FReply CueSelectButton::ButtonAction()
{
	//Do some things...

	return BaseButton::ButtonAction();
}