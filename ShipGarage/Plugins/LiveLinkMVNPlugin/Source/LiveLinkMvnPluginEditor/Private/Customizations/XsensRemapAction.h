// Copyright 2020 Xsens Technologies B.V., Inc. All Rights Reserved.

#pragma once
#include "Settings/EditorLoadingSavingSettings.h"
#include "AssetTypeActions_Base.h"
#include "Animation/Skeleton.h"
#include "EditorAnimUtils.h"
#include "IAssetTypeActions.h"

class FXsensRemapAction : public FAssetTypeActions_Base
{
public:
	// Inherited via FAssetTypeActions_Base
	virtual FText GetName() const override { return ParentAction->GetName(); }
	virtual UClass* GetSupportedClass() const override { return USkeleton::StaticClass(); }
	virtual FColor GetTypeColor() const override { return ParentAction->GetTypeColor(); }
	virtual uint32 GetCategories() override { return ParentAction->GetCategories(); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override { ParentAction->OpenAssetEditor(InObjects, EditWithinLevelEditor); }

	//virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	// Upgrade to v4.24
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;

	bool HasActions(const  TArray<UObject*>& InObjects) const { return true; }

	void XsensSubmenu(FMenuBuilder& MenuBuilder, TArray<TWeakObjectPtr<USkeleton>> Skeletons) const;

	// Used on module initialization to get a handle of the rest of skeleton actions.
	void SetParent(TSharedPtr<IAssetTypeActions>InParent) { ParentAction = InParent; }

	// Function responsible for creating Remap Settings UI.
	void RemapAssetAction(TArray<TWeakObjectPtr<USkeleton>> Skeletons, bool InPlace);

	// Function responsible for modifying existing Remap asset.
	void ModifyExisting(FMenuBuilder& MenuBuilder, TArray<TWeakObjectPtr<USkeleton>> Skeletons) const;
	void ModifyExisting_Window(TArray<TWeakObjectPtr<USkeleton>> Skeletons);

	// Callback for when the asset was selected.
	void ModifyExisting_OnAssetSelected(const FAssetData& AssetData, TWeakObjectPtr<USkeleton> Skeletons) const;
	


private:

	// This handle is used to make sure that the default actions remain in menu.
	TSharedPtr<IAssetTypeActions> ParentAction;
};
