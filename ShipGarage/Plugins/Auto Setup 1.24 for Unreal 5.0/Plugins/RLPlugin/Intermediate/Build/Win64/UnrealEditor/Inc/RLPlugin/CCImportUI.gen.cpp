// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "RLPlugin/Private/CCImportUI.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCCImportUI() {}
// Cross Module References
	RLPLUGIN_API UClass* Z_Construct_UClass_UCCImportUI_NoRegister();
	RLPLUGIN_API UClass* Z_Construct_UClass_UCCImportUI();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_RLPlugin();
// End Cross Module References
	void UCCImportUI::StaticRegisterNativesUCCImportUI()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCCImportUI);
	UClass* Z_Construct_UClass_UCCImportUI_NoRegister()
	{
		return UCCImportUI::StaticClass();
	}
	struct Z_Construct_UClass_UCCImportUI_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UCCImportUI_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_RLPlugin,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UCCImportUI_Statics::Class_MetaDataParams[] = {
		{ "AutoExpandCategories", "FTransform" },
		{ "Comment", "/**\n * TODO:\n */" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "CCImportUI.h" },
		{ "ModuleRelativePath", "Private/CCImportUI.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ToolTip", "TODO:" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UCCImportUI_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCCImportUI>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UCCImportUI_Statics::ClassParams = {
		&UCCImportUI::StaticClass,
		"EditorPerProjectUserSettings",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x000800A0u,
		METADATA_PARAMS(Z_Construct_UClass_UCCImportUI_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UCCImportUI_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UCCImportUI()
	{
		if (!Z_Registration_Info_UClass_UCCImportUI.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCCImportUI.OuterSingleton, Z_Construct_UClass_UCCImportUI_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UCCImportUI.OuterSingleton;
	}
	template<> RLPLUGIN_API UClass* StaticClass<UCCImportUI>()
	{
		return UCCImportUI::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UCCImportUI);
	struct Z_CompiledInDeferFile_FID_HostProject_Plugins_RLPlugin_Source_RLPlugin_Private_CCImportUI_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_HostProject_Plugins_RLPlugin_Source_RLPlugin_Private_CCImportUI_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UCCImportUI, UCCImportUI::StaticClass, TEXT("UCCImportUI"), &Z_Registration_Info_UClass_UCCImportUI, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCCImportUI), 2005929054U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_HostProject_Plugins_RLPlugin_Source_RLPlugin_Private_CCImportUI_h_131991357(TEXT("/Script/RLPlugin"),
		Z_CompiledInDeferFile_FID_HostProject_Plugins_RLPlugin_Source_RLPlugin_Private_CCImportUI_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_HostProject_Plugins_RLPlugin_Source_RLPlugin_Private_CCImportUI_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
