// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ActionCue_editor.h"
#include "ActionCue_editorStyle.h"
#include "ActionCue_editorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "SeekButton.h"

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
	Setup_Buttons();

	SeekContent = SNew( SHorizontalBox )
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

	return  SNew( SDockTab )
		.TabRole( ETabRole::NomadTab )
		[
			// Put your tab content here!
			BuildContent_Display()
		];

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

	//This is How, it only took me 5hrs to find!
	return activeTabManager.ToSharedRef()->FindExistingLiveTab( ActionCue_editorTabName ).ToSharedRef();

} 

void FActionCue_editorModule::RepaintTab()
{
	GetTab()->SetContent( BuildContent_Display() );
}

TSharedRef<SBox> FActionCue_editorModule::BuildContent_Display()
{
/* Window layout
|---------------------------------------------|
|  Seek                         |  Details    |
|                               |             |
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
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	.Padding(15.0f)
	[
		//Split the window with 3 rows
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.MaxHeight(525.0f)
		.Padding( 15, 15 )
		[
			//Split the first row into 2 columns
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.MaxWidth(600.0f)
			[
				//Sample Seek content
				//SNew( STextBlock )
				//.Text( FText::FromString( TEXT( "Sample Seek / Audio Navigator" ) ) )
				SeekContent.ToSharedRef()
			]
	
			+SHorizontalBox::Slot()
			.VAlign( VAlign_Top )
			.HAlign( HAlign_Right )
			.MaxWidth(400.0f)
			[
				//Details Content

				SNew( STextBlock )
				.Text( FText::FromString( TEXT( "Details" ) ) )
				
			]
		]

		+SVerticalBox::Slot()
		.MaxHeight(50.0f)
		.Padding( 0, 0 )
		[
			//Cue Tools
			/*
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Tools" ) ) )
			*/
			SNew( SButton )
			.OnClicked_Raw( this, &FActionCue_editorModule::TEMP_ButtonAction )
		]

		+SVerticalBox::Slot()
		.MaxHeight(425.0f)
		.Padding( 15, 15 )
		[
			//Action Cue Select
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Sample Select" ) ) )
		]

	];

	return content;
}

void FActionCue_editorModule::Setup_Buttons()
{
	//Clear the arrays in-case this is a refresh.
	if(seekButtons.Num() != seekButtonsToDisplay )
	{
		seekButtons.Empty();
		seekButtons.AddDefaulted( seekButtonsToDisplay );

		FString mes = "Seek Button Count: " + FString::FromInt( seekButtons.Num() );
		UE_LOG( LogTemp, Warning, TEXT( "%s" ), *mes );

	}
	

}

int FActionCue_editorModule::GetButtonsToDisplay( ButtonTypes buttonTypes )
{
	switch ( buttonTypes )
	{
		case Seek:
			return seekButtonsToDisplay;
	}

	return 0;
}


void FActionCue_editorModule::Build_SeekContent()
{
	//TSharedRef<SHorizontalBox> content = 
		SeekContent = SNew(SHorizontalBox)
	+SHorizontalBox::Slot()
	[
		SNew( SBox )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		[
			SNew( STextBlock )
			.Text( FText::FromString( TEXT( "Im The Replacment" ) ) )

		]
	];

	//SeekContent = content;

}

FReply FActionCue_editorModule::TEMP_ButtonAction()
{
	Build_SeekContent();
	RepaintTab();
/*
	FText WidgetText = FText::FromString( TEXT( "Im The Replacment" ) );
	
	GetTab()->SetContent(
	
		SNew( SBox )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		[
			SNew( STextBlock )
			.Text( WidgetText )
			
			//	SNew(SButton)
			//	.OnClicked_Raw(this, FActionCue_editorModule::TEMP_ButtonAction)
			
			
		]
	
	);
*/
	return FReply::Handled();

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FActionCue_editorModule, ActionCue_editor)