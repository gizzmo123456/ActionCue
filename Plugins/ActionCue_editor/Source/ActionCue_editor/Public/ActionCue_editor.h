// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// ActionCue Editor By Ashley Sands (gizzmo123456) 2018
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FTabManager;
//struct FOptionalSize;

class SBox;
class SDockTab;
class SHorizontalBox;
class SVerticalBox;
class FReply;

class BaseButton;
class SeekButton;
class CueSelectButton;

class ABaseAudioActor;
class AudioUtills;

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


	bool hadFirstRun = false;
	void FirstRun();

	TSharedPtr<FTabManager> activeTabManager;
	/** returns Main Tab Instance. Used to hold all the panels. */
	TSharedRef<SDockTab> GetTab(); //Use '.SetContent' to update the display.

	ABaseAudioActor* selectedAudioActor;
	AudioUtills* audioData;

	/** Update the selected baseAudioActor */
	void SelectionChanged( UObject* obj );
     
	/** Builds and repaints the tab content 
	  * Does not build any panel content (eg. seek content)
	  */
	void RepaintTab();
	  
	/** Build and Returns the main display */
	TSharedRef<SBox> BuildContent_Display();
	
	enum ButtonTypes { Seek, Select };
	enum DetailsContentTypes { ObjName, ClipName, ClipLength, ClipChannels, ClipSampleRate, ClipTotalSamples, CueCount };

	// max value of an audio sample. (i think its much higher) This is used to work out the button value.
	int maxSampleValue = 10000;

	//Min and max size of audio buttons
	float minButtonSize = 25.0f;	//guaranteed min
	float maxButtonSize = 225.0f;	//Not guaranteed, this is the max button size when the button value is 1. so its more when the button value is > 1

	void Setup_Buttons();
	/** Get the amount of buttons to display by button type. */
	int GetButtonsToDisplay( ButtonTypes ButtonTypes );

	/** Update all buttons in group */
	void Update_ButtonsData( ButtonTypes buttonType );
	/** Update a single BaseButton */
	void Update_buttonData(BaseButton* button, int currentButtonId, int maxButtonId);

	/** Draws all buttons of button type to button hold*/
	void DrawButtons( TSharedRef< SHorizontalBox > buttonHold, ButtonTypes buttonType );	//This is so we can cast to BaseButton.
	/** Draws a single BaseButton to button hold*/
	void DrawButton( TSharedRef< SHorizontalBox > buttonHold, BaseButton* buttonToDraw ); 

	void ButtonPressed_Seek( int buttonId );
	void ButtonPressed_Select( int buttonId );
		
	//Seek Content
	TSharedPtr< SHorizontalBox > seekContent;
	void Build_SeekContent();

	int seekButtonsToDisplay = 50;
	TArray< SeekButton* > seekButtons;

	//define the view range using button ids. < 0 is no value. start MUST be less than end.
	int currentSelectedRange_start = -2;
	int currentSelectedRange_end = -1;

	//Cue Select Content
	TSharedPtr< SHorizontalBox > cueSelectContent;
	void Build_CueSelectContent();

	int cueSelectButtonsToDisplay = 100;
	TArray< CueSelectButton* > cueSelectButtons;

	//Details Content
	TSharedPtr< SVerticalBox > detailsContent;   
	void Build_DetailsContent();

	TSharedRef< SHorizontalBox > DetailsRow( FString lable, FString value );

	FString GetDetailsValues( DetailsContentTypes dcType );

	//Main toolbar content
	TSharedPtr< SHorizontalBox > toolbarContent;
	void Build_ToolbarContent();



	///////////Temp!!
	//build temp default view.
	FReply TEMP_ButtonAction();

};