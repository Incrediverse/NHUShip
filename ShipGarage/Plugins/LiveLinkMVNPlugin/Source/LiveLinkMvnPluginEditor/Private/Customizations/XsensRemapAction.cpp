// Copyright 2020 Xsens Technologies B.V., Inc. All Rights Reserved.

#include "XsensRemapAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Subsystems/AssetEditorSubsystem.h"
#else
#include "Toolkits/AssetEditorManager.h"
#endif
#include "EditorStyle.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimBlueprint.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "SSkeletonWidget.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "AssetToolsModule.h"
#include "Factories/LiveLinkMvnRetargetAssetFactory.h"
#include "Components/RichTextBlock.h"
#include "ToolMenuSection.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "XsensRemapAction"

// This is UE 4.23 way of doing things, which no longer works in UE 4.24
//void FXsensRemapAction::GetActions(const  TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) 
//{ 
//	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
//	ParentAction->GetActions(InObjects, MenuBuilder);
//
//	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
//	auto Skeletons = GetTypedWeakObjectPtrs<USkeleton>(InObjects);
//	if (Skeletons.Num() == 1)
//	{
//		// create xsens submenu
//		MenuBuilder.AddSubMenu(
//			LOCTEXT("CreateSkeletonXsensSubmenu", "Xsens"),
//			LOCTEXT("CreateSkeletonXsensSubmenu_ToolTip", "Remapping Functions"),
//			FNewMenuDelegate::CreateSP(this, &FXsensRemapAction::XsensSubmenu, Skeletons),
//			false,
//			FSlateIcon()
//		);
//	}
//}

/// Helpers.

// Saves an asset.
void SaveAsset(FString* PackageName, UObject* Asset)
{
	FString PackageFileName = FPackageName::LongPackageNameToFilename(*PackageName, FPackageName::GetAssetPackageExtension());
	UPackage *AssetPackage = CreatePackage(NULL, **PackageName);
	GEditor->SavePackage(AssetPackage, Asset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
	// Update UE4.25
	//UPackage::SavePackage(AssetPackage, Asset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
}

// Finds all derived classes based on base class name.
TSet< FName > FindAllDerivedSubclasses(FName BaseClassName)
{
	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Use the asset registry to get the set of all class names deriving from Base
	TSet< FName > DerivedNames;
	{
		TArray< FName > BaseNames;
		BaseNames.Add(BaseClassName);

		TSet< FName > Excluded;
		AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
		return DerivedNames;
	}
}

void FilterAssetPicker(FAssetPickerConfig &AssetPickerConfig)
{
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
	AssetPickerConfig.Filter.bRecursivePaths = true;
	AssetPickerConfig.bCanShowFolders = true;
	AssetPickerConfig.bCanShowClasses = true;

	TArray<FName> ClassNames;
	ClassNames.Add(ULiveLinkMvnRetargetAsset::StaticClass()->GetFName());
	TSet<FName> DerivedClassNames;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	//AssetRegistryModule.Get().GetDerivedClassNames(ClassNames, TSet<FName>(), DerivedClassNames);
	DerivedClassNames.Add(UBlueprint::StaticClass()->GetFName());

	for (auto ClassName : DerivedClassNames)
		AssetPickerConfig.Filter.ClassNames.Add(ClassName);
}

void FXsensRemapAction::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	ParentAction->GetActions(InObjects, Section);

	FAssetTypeActions_Base::GetActions(InObjects, Section);
	auto Skeletons = GetTypedWeakObjectPtrs<USkeleton>(InObjects);
	if (Skeletons.Num() == 1)
	{
		// create xsens submenu
		Section.AddSubMenu(
			"CreateSkeletonXsensSubmenu",
			LOCTEXT("CreateSkeletonXsensSubmenu", "Xsens"),
			LOCTEXT("CreateSkeletonXsensSubmenu_ToolTip", "Remapping Functions"),
			FNewMenuDelegate::CreateSP(this, &FXsensRemapAction::XsensSubmenu, Skeletons),
			false,
			FSlateIcon()
		);
	}	
}

void FXsensRemapAction::XsensSubmenu(FMenuBuilder& MenuBuilder, TArray<TWeakObjectPtr<USkeleton>> Skeletons) const
{
	if (Skeletons.Num() == 1)
	{
		MenuBuilder.BeginSection("XsensRemapping", LOCTEXT("XsensRemapHeading", "Remapping"));
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("Skeleton_Remap", "Create Remap Asset"),
				LOCTEXT("Skeleton_RemapTooltip", "Create Retarget Asset and use this skeleton for Remapping."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP(const_cast<FXsensRemapAction*>(this), &FXsensRemapAction::RemapAssetAction, Skeletons, true),
					FCanExecuteAction()
				)
			);
			MenuBuilder.AddMenuEntry(
				LOCTEXT("Skeleton_Remap_With_Path", "Create Remap Asset In Folder"),
				LOCTEXT("Skeleton_RemapTooltip_With_Path", "Create Retarget Asset and use this skeleton for Remapping. Allows user to select a path to save the retarget asset to."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP(const_cast<FXsensRemapAction*>(this), &FXsensRemapAction::RemapAssetAction, Skeletons, false),
					FCanExecuteAction()
				)
			);

			// In case we would want a window selector instead.
			//MenuBuilder.AddMenuEntry(
			//	LOCTEXT("Skeleton_Remap_Modify", "Modify Existing Remap Asset"),
			//	LOCTEXT("Skeleton_Remap_Modify_ToolTip", "Select existing Remap Asset and set skeleton mesh. Creates a new remap data table in case none is set. "),
			//	FSlateIcon(),
			//	FUIAction(
			//		FExecuteAction::CreateSP(const_cast<FXsensRemapAction*>(this), &FXsensRemapAction::ModifyExisting_Window, Skeletons),
			//		FCanExecuteAction()
			//	)
			//); 

			MenuBuilder.AddSubMenu(
				LOCTEXT("Skeleton_Remap_Modify", "Modify Existing Remap Asset"),
				LOCTEXT("Skeleton_Remap_Modify_ToolTip", "Select existing Remap Asset and set skeleton mesh. Creates a new remap data table in case none is set. "),
				FNewMenuDelegate::CreateSP(this, &FXsensRemapAction::ModifyExisting, Skeletons),
				true,
				FSlateIcon()
			);
		}
		MenuBuilder.EndSection();
	}
}

// this function would be useful in case we would want to have a windowed approach
//todo: unused code
void FXsensRemapAction::ModifyExisting_Window(TArray<TWeakObjectPtr<USkeleton>> Skeletons)
{
	// Config for picker list of geometry collections
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &FXsensRemapAction::ModifyExisting_OnAssetSelected, Skeletons[0]);
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
	AssetPickerConfig.Filter.bRecursivePaths = true;
	AssetPickerConfig.bCanShowFolders = true;
	AssetPickerConfig.bCanShowClasses = true;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FName> ClassNames;
	ClassNames.Add(ULiveLinkMvnRetargetAsset::StaticClass()->GetFName());
	TSet<FName> DerivedClassNames;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	AssetRegistryModule.Get().GetDerivedClassNames(ClassNames, TSet<FName>(), DerivedClassNames);
	TArray< FString > ContentPaths;

	for (auto ClassName : DerivedClassNames)
		AssetPickerConfig.Filter.ClassNames.Add(ClassName);


	// Create and show the window
	auto PickerWindow = SNew(SWindow)
		.Title(LOCTEXT("CreateCacheOptions", "Pick Target Geometry Collection"))
		.ClientSize(FVector2D(500, 600))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		]
		];

	GEditor->EditorAddModalWindow(PickerWindow);
	//PickerWindow.Reset();

}

void FXsensRemapAction::ModifyExisting(FMenuBuilder& MenuBuilder, TArray<TWeakObjectPtr<USkeleton>> Skeletons) const
{
	// Filter config
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &FXsensRemapAction::ModifyExisting_OnAssetSelected, Skeletons[0]);
	FilterAssetPicker(AssetPickerConfig);

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	MenuBuilder.AddWidget(
		SNew(SBox)
		.WidthOverride(300.0f)
		.HeightOverride(300.0f)
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		],
		FText::GetEmpty()
	);
};

void FXsensRemapAction::ModifyExisting_OnAssetSelected(const FAssetData& AssetData, TWeakObjectPtr<USkeleton> Skeletons) const
{
	// Determine the starting path. Try to use the most recently used directory
	const FString SkeletonPackagePath = FPackageName::GetLongPackagePath(Skeletons->GetOutermost()->GetPathName());
	const FString SkeletonName = FPackageName::GetShortFName(Skeletons->GetOutermost()->GetName()).ToString();

	UBlueprint* NewLiveLinkMvnRetargetAssetBP;

	// Create new Asset remapping bluerprint.
	//todo: not generates anything, not sure what it does at all
	NewLiveLinkMvnRetargetAssetBP = (UBlueprint*) Cast<UBlueprint>(AssetData.GetAsset());
	if (NewLiveLinkMvnRetargetAssetBP == nullptr || !Cast<ULiveLinkMvnRetargetAsset>(NewLiveLinkMvnRetargetAssetBP->GeneratedClass->GetDefaultObject()))
		return;
	ULiveLinkMvnRetargetAsset* NewLiveLinkMvnRetargetAsset = CastChecked<ULiveLinkMvnRetargetAsset>(NewLiveLinkMvnRetargetAssetBP->GeneratedClass->GetDefaultObject());

	// Set remap asset as selected.
	{
		TArray<UObject*> ObjectsToSync;
		ObjectsToSync.Add(NewLiveLinkMvnRetargetAssetBP);
		GEditor->SyncBrowserToObjects(ObjectsToSync);
	}

	// Assign default values to the new remap asset that we've created.
	NewLiveLinkMvnRetargetAsset->SetSkeleton(Skeletons.Get());

	// Save assets.
	FString PackageName = SkeletonPackagePath / SkeletonName + "_Remap";
	SaveAsset(&PackageName, NewLiveLinkMvnRetargetAssetBP);
	//todo: use MarkPackageDirty instead of SaveAsset, probably rename the object

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->OpenEditorForAsset(NewLiveLinkMvnRetargetAssetBP);
}

void FXsensRemapAction::RemapAssetAction(TArray<TWeakObjectPtr<USkeleton>> Skeletons, bool InPlace)
{
	// Determine the starting path. Try to use the most recently used directory
	FString SkeletonPackagePath = FPackageName::GetLongPackagePath(Skeletons[0]->GetOutermost()->GetPathName());
	const FString SkeletonName = FPackageName::GetShortFName(Skeletons[0]->GetOutermost()->GetName()).ToString();
	ULiveLinkMvnRetargetAssetFactory* RetargetFactory = NewObject<ULiveLinkMvnRetargetAssetFactory>();
	FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();

	UBlueprint* NewLiveLinkMvnRetargetAssetBP;

	// Create new Asset remapping bluerprint.
	if (!InPlace)
	{
		NewLiveLinkMvnRetargetAssetBP = Cast<UBlueprint>(AssetToolsModule.Get().CreateAssetWithDialog(RetargetFactory->GetSupportedClass(), RetargetFactory));
		if (NewLiveLinkMvnRetargetAssetBP)
			SkeletonPackagePath = FPackageName::GetLongPackagePath(NewLiveLinkMvnRetargetAssetBP->GetOutermost()->GetPathName());
	}
	else
	{
//		FString AssetName, PackageName;
//		CreateUniqueAssetName(SkeletonPackagePath / RetargetFactory->GetDefaultNewAssetName(), TEXT(""), PackageName, AssetName);
		NewLiveLinkMvnRetargetAssetBP = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(SkeletonName + "_Remap" /*AssetName*/, SkeletonPackagePath, RetargetFactory->GetSupportedClass(), RetargetFactory));
	}

	if (NewLiveLinkMvnRetargetAssetBP == nullptr)
		return;

	// Set remap asset as selected.
	{
		TArray<UObject*> ObjectsToSync;
		ObjectsToSync.Add(NewLiveLinkMvnRetargetAssetBP);
		GEditor->SyncBrowserToObjects(ObjectsToSync);
	}

	// Assign default values to the new remap asset that we've created.
	ULiveLinkMvnRetargetAsset* NewLiveLinkMvnRetargetAsset = CastChecked<ULiveLinkMvnRetargetAsset>(NewLiveLinkMvnRetargetAssetBP->GeneratedClass->GetDefaultObject());
	NewLiveLinkMvnRetargetAsset->SetSkeleton(Skeletons[0].Get());
	 
	// Save assets.
	FString PackageName = SkeletonPackagePath / SkeletonName + "_Remap";
	SaveAsset(&PackageName, NewLiveLinkMvnRetargetAssetBP);
	//todo: use MarkPackageDirty instead of SaveAsset, probably rename the object

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->OpenEditorForAsset(NewLiveLinkMvnRetargetAssetBP);
	//FAssetEditorManager::Get().OpenEditorForAsset(NewLiveLinkMvnRetargetAssetBP);
};

#undef LOCTEXT_NAMESPACE
