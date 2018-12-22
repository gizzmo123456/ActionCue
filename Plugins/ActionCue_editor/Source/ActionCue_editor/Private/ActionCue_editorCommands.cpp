// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ActionCue_editorCommands.h"

#define LOCTEXT_NAMESPACE "FActionCue_editorModule"

void FActionCue_editorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ActionCue_editor", "Bring up ActionCue_editor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
