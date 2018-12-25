// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ActionCue_editor.h"
#include "ActionCue_editorStyle.h"
#include "ActionCue_editorCommands.h"
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

	///////////// TEMP place holds
	seekContent = SNew( SHorizontalBox )
	+ SHorizontalBox::Slot()
	[
		SNew( SBox )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		[
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Im The Seek Content" ) ) )

		]
	];

	cueSelectContent = SNew( SHorizontalBox )
	+ SHorizontalBox::Slot()
	[
		SNew( SBox )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		[
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Im The Cue Select Content" ) ) )
		]
	];

	detailsContent = SNew( SVerticalBox )
	+ SVerticalBox::Slot()
	[
		SNew( SBox )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		[
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Im The details content" ) ) )
		]
	];

	toolbarContent = SNew( SHorizontalBox )
	+SHorizontalBox::Slot()
	.MaxWidth(45.0f)
	.Padding(5.0f, 5.0f)
	[
		SNew( SButton )
		.OnClicked_Raw( this, &FActionCue_editorModule::TEMP_ButtonAction )
		.Text(FText::FromString(FString("Load")))
	];

	/////////// Eof TEMP place holds

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

	if ( selectedAudio == nullptr ) return;	// No BaseAudioActor selected

	selectedAudioActor = selectedAudio;

	FString msg = "Selection Changed: " + selectedAudioActor->GetName();// GetFullName();
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

	//Create the main content hold
	TSharedRef<SBox> content = SNew( SBox )
	.VAlign( VAlign_Fill )
	.HAlign( HAlign_Fill )
	.Padding(15.0f)
	[
		//Split the window with 3 rows
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.MaxHeight(625.0f)
		.Padding( 15.0f, 125.0f, 15.0f, -25.0f )
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
			.Padding(0.0f, -100.0f, 0.0f, 0.0f)
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
		.Padding( 15.0f, 125.0f, 15.0f, -75.0f)
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
			seekButtons.Add( new SeekButton() );
		//seekButtons.AddDefaulted( seekButtonsToDisplay );

		FString mes = "Seek Button Count: " + FString::FromInt( seekButtons.Num() );
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *mes );

	}
	
	if ( cueSelectButtons.Num() != cueSelectButtonsToDisplay )
	{
		cueSelectButtons.Empty();
		for ( int i = 0; i < cueSelectButtonsToDisplay; i++ )
			cueSelectButtons.Add( new CueSelectButton() );

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
	TSharedRef< SVerticalBox > buttonHeightBox = SNew( SVerticalBox );
	SVerticalBox::FSlot& vSlot = buttonHeightBox->AddSlot()
		.MaxHeight( 75.0f + ( 150.0f * button->GetValue() ) )
		.Padding( 0.0f, (75.0f + ( 150.0f * ( 1.0f - button->GetValue() ) ) ) / 2.0f )
		[
			button->GetButton()
		];

	// Create a new horizontal slot in the button hold so all the buttons have the same width and horizontal spacing
	// and insert the vertical box holding the button
	SHorizontalBox::FSlot& hSlot = buttonHold->AddSlot()
		//.MaxWidth( 250.0f )
		.VAlign(VAlign_Fill)
		.Padding( 3.0f, -150.0f )
		[
			buttonHeightBox
		];

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
	+SVerticalBox::Slot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Object Name", GetDetailsValues( DetailsContentTypes::ObjName ) )
	]
	+SVerticalBox::Slot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Name", GetDetailsValues( DetailsContentTypes::ClipName ) )
	]
	+SVerticalBox::Slot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Length", GetDetailsValues( DetailsContentTypes::ClipLength ) )
	]
	+SVerticalBox::Slot()
	.Padding(50.0f, 5.0f, 5.0f, 5.0f)
	[
		DetailsRow("Clip Channels", GetDetailsValues( DetailsContentTypes::ClipChannels ) )
	]
	+SVerticalBox::Slot()
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
	.MaxWidth( 55.0f )
	.Padding( 5.0f, 5.0f )
	[
		SNew( SButton )
		.OnClicked_Raw( this, &FActionCue_editorModule::TEMP_ButtonAction )
		[
			SNew( STextBlock )
			.Text( FText::FromString( "Refresh" ) )
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

		case DetailsContentTypes::CueCount:
			return "0";	//Todo: implement cue.

	}

	return "Error: Details content not found.";
}

FReply FActionCue_editorModule::TEMP_ButtonAction()
{
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