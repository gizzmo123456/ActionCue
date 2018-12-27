// Ashley M Sands (gizzmo123456) 2018

#pragma once

#include "CoreMinimal.h"
#include "BaseButton.h"

/**
 *		Seek audio button
 */
class ACTIONCUE_EDITOR_API SeekButton : public BaseButton
{

	

public:
	SeekButton(int bid);
	virtual ~SeekButton();

protected:
	virtual FSlateColor GetButtonColour() const override;	//Button color callback.
	virtual FReply ButtonAction() override;					//Button Action callback.

};
