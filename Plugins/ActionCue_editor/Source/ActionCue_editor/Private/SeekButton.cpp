// Ashley M Sands (gizzmo123456) 2018

#include "SeekButton.h"

SeekButton::SeekButton():BaseButton()
{
	SetValue(FMath::RandRange(0.0f, 1.0f));
	
	/*
	TSharedRef<SButton> helloo_button = SNew( SButton )
		.OnClicked_Raw( this, &SeekButton::ButtonAction )
		.ButtonColorAndOpacity_Raw( this, &SeekButton::GetButtonColour );
	*/
		
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