// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ActionCue_editor.h"
#include "ActionCue_editorStyle.h"
#include "ActionCue_editorCommands.h"
#include "AudioUtills.h"

#include "LevelEditor.h"
#include "Editor.h"

#include "Engine.h"
#include "Engine/Selection.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

#include "Widgets/SBoxPanel.h"			//used ??

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "SeekButton.h"
#include "CueSelectButton.h"

#include "BaseAudioActor.h"

static const FName ActionCue_editorTabName("ActionCue_editor");
//TSharedPtr<SDockTab> FActionCue_editorModule::main__t;


#define LOCTEXT_NAMESPACE "FActionCue_editorModule"

void FActionCue_editorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FActionCue_editorStyle::Initialize();
	FActionCue_editorStyle::ReloadTextures();

	FActionCue_editorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FActionCue_editorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FActionCue_editorModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FActionCue_editorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FActionCue_editorModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FTabSpawnerEntry tse = FGlobalTabmanager::Get()->RegisterNomadTabSpawner( ActionCue_editorTabName, FOnSpawnTab::CreateRaw( this, &FActionCue_editorModule::OnSpawnPluginTab ) )
		.SetDisplayName( LOCTEXT( "FActionCue_editorTabTitle", "ActionCue_editor" ) )
		.SetMenuType( ETabSpawnerMenuType::Hidden );

}

void FActionCue_editorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FActionCue_editorStyle::Shutdown();

	FActionCue_editorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ActionCue_editorTabName);
}

TSharedRef<SDockTab> FActionCue_editorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{

	FirstRun();

	// Create the basic button data
	Update_ButtonsData( ButtonTypes::Seek );
	Update_ButtonsData( ButtonTypes::Select );

	// Build all content
	Build_SeekContent();
	Build_CueSelectContent();
	Build_DetailsContent();
	Build_ToolbarContent();

	return  SNew( SDockTab )
		.TabRole( ETabRole::NomadTab )
		[
			// Put your tab content here!
			BuildContent_Display()
		];

}

void FActionCue_editorModule::FirstRun()
{
	if ( hadFirstRun ) return;

	audioData = new AudioUtills();

	Setup_Buttons();

	// Register delegates
	USelection::SelectionChangedEvent.AddRaw( this, &FActionCue_editorModule::SelectionChanged );
	

	hadFirstRun = true;

}

void FActionCue_editorModule::PluginButtonClicked()
{
	activeTabManager = FGlobalTabmanager::Get()->InvokeTab( ActionCue_editorTabName )->GetTabManager();
}

void FActionCue_editorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FActionCue_editorCommands::Get().OpenPluginWindow);
}

void FActionCue_editorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FActionCue_editorCommands::Get().OpenPluginWindow);
}

TSharedRef<SDockTab> FActionCue_editorModule::GetTab()
{
	//A bit of a work around to return the tab instance, its just that the tab blinks when ever we need to make an update :|
	//I've done it like this because if i store the tab in a var the window will NOT reopen once its been closed.
	//There must be a better way! //Like the function after sounds good but does not return the correct window! hmmm...
	//Have a look at InvokeTab!!
	//return FGlobalTabmanager::Get()->InvokeTab( ActionCue_editorTabName ); //FGlobalTabmanager::Get()->FindExistingLiveTab( ActionCue_editorTabName ).ToSharedRef();

	//Check there is a valid tab manager
	if ( !activeTabManager.IsValid() )
		PluginButtonClicked();

	//This is How, it only took me 5hrs to find!
	return activeTabManager.ToSharedRef()->FindExistingLiveTab( ActionCue_editorTabName ).ToSharedRef();

} 

void FActionCue_editorModule::SelectionChanged( UObject* obj )
{
	//Check that there is at least one actor selected in the scene
	if ( GEditor->GetSelectedActorCount() == 0 ) return;

	//Get the last object that was selected that inherits form ABaseAudioActor
	ABaseAudioActor* selectedAudio = GEditor->GetSelectedActors()->GetBottom<ABaseAudioActor>();

	if ( selectedAudio == selectedAudioActor ) return;	// Nothing has really changed.

	// Flag that we need to refresh the amp data
	hasSeekAmpData = false;
	hasSelectAmpData = false;
	
	selectedAudioActor = selectedAudio;

	USoundWave* audioClip = ( !selectedAudio ? nullptr : selectedAudioActor->audioClip );	// prevent crash if selectedAudio is null
	audioData->SetAudioClip( audioClip );

	/*
	** Rebuild data and repaint tab
	** Todo. this all needs its own function??
	*/

	//refresh button data
	Update_ButtonsData( ButtonTypes::Seek );
	Update_ButtonsData( ButtonTypes::Select );

	// Build all content
	Build_SeekContent();
	Build_CueSelectContent();
	Build_DetailsContent();
	RepaintTab();

	FString audioActorName = ( !selectedAudio ? "None" : selectedAudioActor->GetName() );
	FString msg = "Selection Changed: " + audioActorName;// GetFullName();
	UE_LOG( LogTemp, Warning, TEXT( "%s" ), *msg );



}

void FActionCue_editorModule::RepaintTab()
{
	GetTab()->SetContent( BuildContent_Display() );
	GetTab()->CacheVolatility();
}

TSharedRef<SBox> FActionCue_editorModule::BuildContent_Display()
{
/* Window layout
|---------------------------------------------|
|  Seek                         |  Details    |
|                               |             |
|-------------------------------|-------------|
|<| Seek Bar / zoom           |>|             |
|---------------------------------------------|
|  Cue Tool bar                               |
|---------------------------------------------|
|  Cue Select                                 |
|                                             |
|                                             |
|---------------------------------------------|
*/

	float audioTopPadding = (maxButtonSize - minButtonSize) + 25.0f;

	//Create the main content hold
	TSharedRef<SBox> content = SNew( SBox )
	.VAlign( VAlign_Fill )
	.HAlign( HAlign_Fill )
	.Padding(15.0f)
	[
		//Split the window with 3 rows
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.MaxHeight( 75.0f )
		.Padding( 15.0f, 25.0f, 15.0f, -25.0f )
		[
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Action Cue BETA v0.4 By Ashley Sands" ) ) )
		]
		+SVerticalBox::Slot()
		.MaxHeight(625.0f)
		.Padding( 15.0f, audioTopPadding, 15.0f, 10.0f )
		[
			//Split the first row into 2 columns
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			.MaxWidth(1320.0f)
			.Padding(0.0f, 0.0f, 0.0f, 0.0f)
			[
				//Sample Seek content
				seekContent.ToSharedRef()
			]
	
			+SHorizontalBox::Slot()
			.VAlign( VAlign_Top )
			.HAlign( HAlign_Fill )
			.Padding(0.0f, -(audioTopPadding+25.0f), 0.0f, 0.0f)
			.MaxWidth(1000.0f)
			[
				//Details Content
				detailsContent.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.MaxHeight( 25.0f )
		.VAlign(VAlign_Bottom)
		.Padding( 0.0f, 100.0f, 0.0f, -50.0f )
		[
			// Seek bar
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Seek Bar" ) ) )
		]
		+SVerticalBox::Slot()
		.MaxHeight(35.0f)
		.Padding( 0.0f, 50.0f, 0.0f, 20.0f )
		[
			//Cue Tools
			toolbarContent.ToSharedRef()
		]

		+SVerticalBox::Slot()
		.MaxHeight(325.0f)
		.Padding( 15.0f, audioTopPadding, 15.0f, 25.0f)
		.VAlign(VAlign_Fill)
		[
			//Action Cue Select
			cueSelectContent.ToSharedRef()
		]
	];
	
	return content;
}

void FActionCue_editorModule::Setup_Buttons()
{
	//Only update the arrays if the amount of buttons has changed
	if(seekButtons.Num() != seekButtonsToDisplay )
	{
		seekButtons.Empty();
		for ( int i = 0; i < seekButtonsToDisplay; i++ )
		{
			seekButtons.Add( new SeekButton( i ) );
			seekButtons[i]->OnButtonPressed.AddRaw( this, &FActionCue_editorModule::ButtonPressed_Seek);
		}
		//seekButtons.AddDefaulted( seekButtonsToDisplay );

		FString mes = "Seek Button Count: " + FString::FromInt( seekButtons.Num() );
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *mes );

	}
	
	if ( cueSelectButtons.Num() != cueSelectButtonsToDisplay )
	{
		cueSelectButtons.Empty();
		for ( int i = 0; i < cueSelectButtonsToDisplay; i++ )
		{
			cueSelectButtons.Add( new CueSelectButton( i ) );
			cueSelectButtons[i]->OnButtonPressed.AddRaw( this, &FActionCue_editorModule::ButtonPressed_Select );
		}

		FString mes = "Cue Select Button Count: " + FString::FromInt( cueSelectButtons.Num() );
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *mes );

	}

}

int FActionCue_editorModule::GetButtonsToDisplay( ButtonTypes buttonTypes )
{
	switch ( buttonTypes )
	{
		case Seek:
			return seekButtonsToDisplay;
		case Select:
			return cueSelectButtonsToDisplay;
	}

	return 0;
}

void FActionCue_editorModule::Update_ButtonsData( ButtonTypes buttonType )
{

	int buttonCount = GetButtonsToDisplay( buttonType );
	BaseButton* button;

	int nectActionCueId = -1;		//used when updating the cue select to update the selected buttons


	//Extract the button from the array of button type
	for ( int i = 0; i < buttonCount; i++ )
	{
		switch ( buttonType )
		{
			case ButtonTypes::Seek:

				{
					int endSample = seekButtons[seekButtons.Num() - 1]->GetSample( BaseButton::SampleRangeType::End );

					if ( !hasSeekAmpData )
						endSample = audioData->totalSamples - 1;

					button = seekButtons[i];
					Update_buttonData( button, i, buttonCount, 0, endSample );

				}
				break;

			case ButtonTypes::Select:

				{
					button = cueSelectButtons[i];
					//check the current buttons are in range if not set them to display all.
					int selectedStart = currentSelectedRange_start;
					int selectedEnd = currentSelectedRange_end;

					if ( selectedStart < 0 && selectedEnd >= 0 )
						selectedStart = selectedEnd;
					else if ( selectedStart < 0 )
						selectedStart = 0;

					if ( selectedStart > 0 && selectedEnd < 0 )
						selectedEnd = selectedStart;
					else if ( selectedEnd < 0 )
						selectedEnd = seekButtons.Num() - 1;

					//transform the button id's into sample range
					int selectedStartRange = seekButtons[selectedStart]->GetSample( BaseButton::SampleRangeType::Start );
					int selectedEndRange = seekButtons[selectedEnd]->GetSample( BaseButton::SampleRangeType::End );

					if ( !hasSelectAmpData )
					{
						selectedStartRange = 0;
						selectedEndRange = audioData->totalSamples - 1;
					}

					Update_buttonData( button, i, buttonCount, selectedStartRange, selectedEndRange );
					nectActionCueId = Update_ButtonIsSet( button, nectActionCueId, selectedStartRange, selectedEndRange );
				}

				break;

		}
	}

	//we must set hasSeekAmpData at the end so they all get set
	if ( buttonType == ButtonTypes::Seek )
		hasSeekAmpData = true;
	else if ( buttonType == ButtonTypes::Select )
		hasSelectAmpData = true;

}

void FActionCue_editorModule::Update_buttonData( BaseButton* button, int currentButtonId, int maxButtonId, int startSampleRange, int endSampleRange )
{
	//make shore that the start and end samples are in range.
	if ( startSampleRange >= audioData->totalSamples )
		startSampleRange = audioData->totalSamples - 1;

	if ( endSampleRange < startSampleRange )
		endSampleRange = startSampleRange;

	if ( endSampleRange >= audioData->totalSamples )
		endSampleRange = audioData->totalSamples - 1;

	//find the sample range
	int totalSamplesInRange = endSampleRange - startSampleRange;
	int samplesRange = FMath::CeilToInt( totalSamplesInRange / maxButtonId );

	int startSample = startSampleRange + (samplesRange * currentButtonId);
	int endSample = startSample + samplesRange;

	// Make shore the start and end samples do not go out of range of the audio samples
	if ( startSample >= audioData->totalSamples )
		startSample = endSample = audioData->totalSamples - 1;		//End sample is alway more than start sample
	else if ( endSample >= audioData->totalSamples )
		endSample = audioData->totalSamples - 1;
	
	FString s = "StartSamp: " + FString::FromInt( startSampleRange ) + " EndSamp: " + FString::FromInt( endSampleRange ) + " TotalSamp: "+FString::FromInt(audioData->totalSamples);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *s)

	button->SetSampleRange( startSample, endSample );
	button->SetValue( audioData->GetAmplitudeData( startSample, endSample ) / maxSampleValue );

}

int FActionCue_editorModule::Update_ButtonIsSet( BaseButton* button, int actionCueId, int startSample, int endSample )
{
	if ( !selectedAudioActor ) return -1;
	FString s = "";  //debuging

	//find the first action key
	if ( actionCueId == -1 )
	{		
		bool found = false;		// so we can return if no action is found to be in range.

		//find which action if any is in range
		for ( int i = 0; i < selectedAudioActor->actionCues.Num(); i++ )
		{
			int actionCueSample = audioData->SecondsToSamples( selectedAudioActor->actionCues[i] );

			if ( actionCueSample >= startSample && actionCueSample <= endSample )
			{
				actionCueId = i;
				found = true;
				break;
			}

		}
		
		if(!found)
		{
			actionCueId = -2;
		}

		s = ( found ? "true" : "false");

	}

	s = "<<>> " + FString::FromInt( actionCueId ) + " :: " + s;
	UE_LOG( LogTemp, Log, TEXT( "%s" ), *s );

	// Set the button to its correct state.
	if ( actionCueId >= 0 )
	{
		int actionCueSample = audioData->SecondsToSamples( selectedAudioActor->actionCues[actionCueId] );

		if ( button->IsSampleInRange( actionCueSample ) )
		{
			button->Set( true );
			actionCueId++;
			// check that the action cue is still in range
			if ( actionCueId >= selectedAudioActor->actionCues.Num() )
				actionCueId = 0;
		}
		else
		{
			button->Set( false );
		}

	}
	else
	{
		button->Set( false );
	}

	return actionCueId;
}

void FActionCue_editorModule::DrawButtons( TSharedRef<SHorizontalBox> buttonHold, ButtonTypes buttonType )
{
	int buttonCount = GetButtonsToDisplay( buttonType );
	BaseButton* button;

	//Extract the button from the array of button type
	for ( int i = 0; i < buttonCount; i++ )
	{
		switch(buttonType)
		{
			case ButtonTypes::Seek:
				button = seekButtons[ i ];
				DrawButton( buttonHold, button );
				break;
			case ButtonTypes::Select:
				button = cueSelectButtons[ i ];
				DrawButton( buttonHold, button );
				break;
		}
	}

}

void FActionCue_editorModule::DrawButton( TSharedRef< SHorizontalBox > buttonHold, BaseButton* button )
{

	// Todo: For the min width issue i think the padding should be set via a delegate
	// so that the padding does not got below the HBox max width. 

	// Create a vertical box so the button is the correct height
	// we do this for each button as there all different heights

	float sizeDif = maxButtonSize - minButtonSize;

	TSharedRef< SVerticalBox > buttonHeightBox = SNew( SVerticalBox );
	SVerticalBox::FSlot& vSlot = buttonHeightBox->AddSlot()
		.MaxHeight( minButtonSize + ( sizeDif * button->GetValue() ) )
		.Padding( 0.0f, (minButtonSize + ( sizeDif * ( 1.0f - button->GetValue() ) ) ) / 2.0f )
		[
			button->GetButton()
		];

	// Create a new horizontal slot in the button hold so all the buttons have the same width and horizontal spacing
	// and insert the vertical box holding the button
	SHorizontalBox::FSlot& hSlot = buttonHold->AddSlot()
		//.MaxWidth( 250.0f )
		.VAlign(VAlign_Fill)
		.Padding( 3.0f, -sizeDif )
		[
			buttonHeightBox
		];

}

void FActionCue_editorModule::ButtonPressed_Seek( int buttonId )
{
	//Find if its the start or end button that has been changed.
	if ( buttonId == currentSelectedRange_start )
	{
		currentSelectedRange_start = -1;
	}
	else if ( buttonId == currentSelectedRange_end )
	{
		if ( currentSelectedRange_start >= 0 )
		{
			currentSelectedRange_end = currentSelectedRange_start;
			currentSelectedRange_start = -1;
		}
		else
		{
			currentSelectedRange_end = -1;
		}
	}
	else if ( buttonId < currentSelectedRange_end )
	{
		if(currentSelectedRange_start >= 0)
			seekButtons[currentSelectedRange_start]->Set( false );
		currentSelectedRange_start = buttonId;
	}
	else if ( buttonId > currentSelectedRange_end )
	{
		if ( currentSelectedRange_end >= 0 )
			seekButtons[currentSelectedRange_end]->Set( false );
		currentSelectedRange_end = buttonId;
	}

	FString s = "Seek button pressed: " + FString::FromInt( buttonId ) + " || Current selected start: "+FString::FromInt(currentSelectedRange_start)+" Current Selected end:"+FString::FromInt(currentSelectedRange_end);
	UE_LOG( LogTemp, Log, TEXT( "%s" ), *s );

}

void FActionCue_editorModule::ButtonPressed_Select( int buttonId )
{
	//Find the time position of the selected button.
	int sample = cueSelectButtons[buttonId]->GetAvgSample();
	float time = audioData->SamplesToSeconds(sample);

	
	bool addKey = cueSelectButtons[buttonId]->IsSet();
	Update_SelectedAudioActorActions( time, addKey, buttonId );
	
	FString s = "Select button pressed: " + FString::FromInt( buttonId ) +" Sample: "+FString::FromInt(sample)+" Time: "+FString::SanitizeFloat(time);
	UE_LOG( LogTemp, Log, TEXT( "%s" ), *s );

}

void FActionCue_editorModule::Update_SelectedAudioActorActions( float time, bool addKey, int buttonId )
{
	//Error: nothing to remove
	if ( !addKey && selectedAudioActor->actionCues.Num() == 0 ) 
	{
		FString s = "Selected audio actor has no actions to remove";
		UE_LOG( LogTemp, Error, TEXT( "%s" ), *s );
		return;
	}
	
	//Add the first element to the array of actions
	if ( addKey &&  selectedAudioActor->actionCues.Num() == 0 ) 
	{
		selectedAudioActor->actionCues.Add( time );
		return;
	}

	// So theres actions, find where to add or what to remove
	//Todo. Add undo step. See the unreal plugin base script that is after standalone for more info.

	bool done = false;		//so we can print a message if it fails to add or remove.

	for ( int i = 0; i < selectedAudioActor->actionCues.Num(); i++ )
	{
		if ( addKey && (time < selectedAudioActor->actionCues[ i ] || i == selectedAudioActor->actionCues.Num()-1) )
		{
			//Add it to the end if its after time
			if( time > selectedAudioActor->actionCues[i] )
				selectedAudioActor->actionCues.Add( time );
			else
				selectedAudioActor->actionCues.Insert( time, i );

			done = true;
			break;
		}
		else if ( !addKey && cueSelectButtons[buttonId]->IsSampleInRange( audioData->SecondsToSamples( selectedAudioActor->actionCues[i] ) ) )
		{
			selectedAudioActor->actionCues.RemoveAt( i );
			done = true;
			break;
		}
	}

	if ( !done )
	{
		FString s = ( addKey ? "add" : "remove" );
		s = "Failed to " + s + " key at time: " + FString::SanitizeFloat( time );
		UE_LOG( LogTemp, Error, TEXT( "%s" ), *s );
	}

}

void FActionCue_editorModule::Build_SeekContent()
{
	// Create a new seek content hold and generate its contents
	seekContent = SNew( SHorizontalBox );
	DrawButtons( seekContent.ToSharedRef(), ButtonTypes::Seek );
}

void FActionCue_editorModule::Build_CueSelectContent()
{
	// Create a new cue select content hold and generate its contents.
	cueSelectContent = SNew( SHorizontalBox );
	DrawButtons( cueSelectContent.ToSharedRef(), ButtonTypes::Select );
}

void FActionCue_editorModule::Build_DetailsContent()
{
	// Create a new cue select content hold and generate its contents.
	detailsContent = SNew( SVerticalBox )
	+ SVerticalBox::Slot()
	.Padding( 50.0f, 5.0f, 5.0f, 5.0f )
	[
		DetailsRow( "Object Name", GetDetailsValues( DetailsContentTypes::ObjName ) )
	];

	// don't display any audio data if theres no BaseAudioActor selected
	if ( selectedAudioActor == nullptr) return;

	detailsContent->AddSlot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Name", GetDetailsValues( DetailsContentTypes::ClipName ) )
	];
	
	// Only display audio data if there is an audio clip
	if ( selectedAudioActor->audioClip == nullptr ) return;

	detailsContent->AddSlot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Length", GetDetailsValues( DetailsContentTypes::ClipLength ) )
	];

	detailsContent->AddSlot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Channels", GetDetailsValues( DetailsContentTypes::ClipChannels ) )
	];

	detailsContent->AddSlot()
	.Padding( 50.0f, 5.0f, 5.0f, 5.0f )
	[
		DetailsRow( "Clip Sample Rate", GetDetailsValues( DetailsContentTypes::ClipSampleRate ) )
	];

	detailsContent->AddSlot()
	.Padding( 50.0f, 5.0f, 5.0f, 5.0f )
	[
		DetailsRow( "Clip Total Samples", GetDetailsValues( DetailsContentTypes::ClipTotalSamples ) )
	];

	detailsContent->AddSlot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Cue Count", GetDetailsValues(DetailsContentTypes::CueCount))
	];

}

TSharedRef<SHorizontalBox> FActionCue_editorModule::DetailsRow( FString lable, FString value )
{
	TSharedRef<SHorizontalBox> row = SNew( SHorizontalBox )
	+SHorizontalBox::Slot()
	.HAlign(HAlign_Left)
	.MaxWidth(175.0f)
	[
		SNew( STextBlock )
		.Text( FText::FromString( lable ) )
	]
	+SHorizontalBox::Slot()
		.HAlign( HAlign_Left )
		.MaxWidth( 1800.0f )
	[
		SNew( STextBlock )
		.Text( FText::FromString( value ) )
	];

	return row;
}

void FActionCue_editorModule::Build_ToolbarContent()
{

	toolbarContent = SNew( SHorizontalBox )
	+ SHorizontalBox::Slot()
	.MaxWidth( 55.0f )
	.Padding( 5.0f, 5.0f )
	[
		SNew( SButton )
		.OnClicked_Raw( this, &FActionCue_editorModule::TEMP_ButtonAction )
		.Text( FText::FromString( FString( "Load" ) ) )

	] 
	+ SHorizontalBox::Slot()
	.MaxWidth( 155.0f )
	.Padding( 5.0f, 5.0f )
	[
		SNew( SButton )
		.OnClicked_Raw( this, &FActionCue_editorModule::RefreshContent_select )
		[
			SNew( STextBlock )
			.Text( FText::FromString( "Refresh Action Select" ) )
		]
	];

}

FString FActionCue_editorModule::GetDetailsValues( DetailsContentTypes dcType )
{

	// return our default values for none selected or no audio clip
	if ( selectedAudioActor == nullptr )
		return "None Selected";
	else if ( selectedAudioActor->audioClip == nullptr && dcType != DetailsContentTypes::ObjName )
		return "None";


	switch ( dcType )
	{
		case DetailsContentTypes::ObjName:
			return selectedAudioActor->GetName();

		case DetailsContentTypes::ClipName:
			return  selectedAudioActor->audioClip->GetName();

		case DetailsContentTypes::ClipLength:
			return  FString::SanitizeFloat( selectedAudioActor->audioClip->GetDuration(), 0) + " seconds";

		case DetailsContentTypes::ClipChannels:
			return FString::FromInt( selectedAudioActor->audioClip->NumChannels );

		case DetailsContentTypes::ClipSampleRate:
			return FString::FromInt( audioData->sampleRate );

		case DetailsContentTypes::ClipTotalSamples:
			return FString::FromInt( audioData->totalSamples );

		case DetailsContentTypes::CueCount:
			return "0";	//Todo: implement cue.

	}

	return "Error: Details content not found.";
}

FReply FActionCue_editorModule::RefreshContent_select()
{
	Update_ButtonsData( ButtonTypes::Select );
	Build_CueSelectContent();
	RepaintTab();

	return FReply::Handled();
}

FReply FActionCue_editorModule::TEMP_ButtonAction()
{
	//Update data
	Update_ButtonsData( ButtonTypes::Seek );
	Update_ButtonsData( ButtonTypes::Select );

	//Build all content
	Build_SeekContent();
	Build_CueSelectContent();
	Build_DetailsContent();
	Build_ToolbarContent();

	//Repaint the tab
	RepaintTab();

	return FReply::Handled();

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActionCue_editorModule, ActionCue_editor)