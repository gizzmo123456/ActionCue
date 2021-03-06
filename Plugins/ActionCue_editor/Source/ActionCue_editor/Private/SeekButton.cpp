// Ashley M Sands (gizzmo123456) 2018

#include "SeekButton.h"

SeekButton::SeekButton(int bid):BaseButton(bid)
{
	SetValue( 0 );		
}

SeekButton::~SeekButton()
{
}

FSlateColor SeekButton::GetButtonColour() const
{
	FLinearColor col = IsSet() ? FLinearColor( 1.0f, 1.0f, 0.0f ) : FLinearColor( 0.0f, 1.0f, 1.0f );
	return col;
}

FReply SeekButton::ButtonAction()
{
	//Do some things...

	return BaseButton::ButtonAction();
}