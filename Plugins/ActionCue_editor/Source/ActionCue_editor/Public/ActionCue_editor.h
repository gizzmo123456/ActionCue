// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// ActionCue Editor By Ashley Sands (gizzmo123456) 2018
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FTabManager;
class SBox;
class SDockTab;
class FReply;

class FActionCue_editorModule : public IModuleInterface
{
/////// Default standalone editor
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<class FUICommandList> PluginCommands;

/////// ActionCue Editor
//public:
//protected:
private:

	TSharedPtr<FTabManager> activeTabManager;
	/** returns Main Tab Instance. Used to hold all the panels. */
	TSharedRef<SDockTab> GetTab(); //Use '.SetContent' to update the display.

	/** Build and Returns the main display */
	TSharedRef<SBox> BuildContent_Display();
	
	//Todo:
	//Build Seek Content
	//Build Details Content
	//Build Cue Select Content
	//Build tool bars??

	///////////Temp!!
	//build temp default view.
	FReply TEMP_ButtonAction();  
};