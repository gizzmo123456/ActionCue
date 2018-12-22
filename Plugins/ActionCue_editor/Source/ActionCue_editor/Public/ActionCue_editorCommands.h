// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ActionCue_editorStyle.h"

class FActionCue_editorCommands : public TCommands<FActionCue_editorCommands>
{
public:

	FActionCue_editorCommands()
		: TCommands<FActionCue_editorCommands>(TEXT("ActionCue_editor"), NSLOCTEXT("Contexts", "ActionCue_editor", "ActionCue_editor Plugin"), NAME_None, FActionCue_editorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};