// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"
#include "BaseButton.h"

/**
 * 
 */
class ACTIONCUE_EDITOR_API SeekButton : public BaseButton
{
public:
	SeekButton();
	virtual ~SeekButton();

protected:
	virtual FSlateColor GetButtonColour() override;	//Button color callback.
	virtual FReply ButtonAction() override;					//Button Action callback.

};
