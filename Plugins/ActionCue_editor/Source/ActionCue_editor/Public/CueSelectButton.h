// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"
#include "BaseButton.h"

/**
 *		Select Action Cue point buttons.
 */
class ACTIONCUE_EDITOR_API CueSelectButton : public BaseButton
{
public:
	CueSelectButton(int bid); 
	virtual ~CueSelectButton();

protected:
	virtual FSlateColor GetButtonColour() const override;	//Button color callback.
	virtual FReply ButtonAction() override;					//Button Action callback.


};
