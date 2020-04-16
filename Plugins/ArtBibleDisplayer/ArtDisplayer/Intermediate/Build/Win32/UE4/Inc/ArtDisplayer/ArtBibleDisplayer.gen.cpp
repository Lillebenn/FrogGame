// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ArtDisplayer/Public/ArtBibleDisplayer.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeArtBibleDisplayer() {}
// Cross Module References
	ARTDISPLAYER_API UClass* Z_Construct_UClass_AArtBibleDisplayer_NoRegister();
	ARTDISPLAYER_API UClass* Z_Construct_UClass_AArtBibleDisplayer();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_ArtDisplayer();
	ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
// End Cross Module References
	void AArtBibleDisplayer::StaticRegisterNativesAArtBibleDisplayer()
	{
	}
	UClass* Z_Construct_UClass_AArtBibleDisplayer_NoRegister()
	{
		return AArtBibleDisplayer::StaticClass();
	}
	struct Z_Construct_UClass_AArtBibleDisplayer_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bCameraZOffset_MetaData[];
#endif
		static void NewProp_bCameraZOffset_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bCameraZOffset;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Index_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Index;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NextIndex_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_NextIndex;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_DelayTime_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_DelayTime;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RotationSpeed_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_RotationSpeed;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bFullRotation_MetaData[];
#endif
		static void NewProp_bFullRotation_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bFullRotation;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bRotate_MetaData[];
#endif
		static void NewProp_bRotate_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bRotate;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bLoop_MetaData[];
#endif
		static void NewProp_bLoop_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bLoop;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAutoDisplay_MetaData[];
#endif
		static void NewProp_bAutoDisplay_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAutoDisplay;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CameraBoom_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CameraBoom;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AutoFillFolders_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_AutoFillFolders;
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_AutoFillFolders_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticMeshes_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_StaticMeshes;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticMeshes_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_DisplayedObject_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_DisplayedObject;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AArtBibleDisplayer_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_ArtDisplayer,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "ArtBibleDisplayer.h" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Should the camera boom offset its Z position to account for a low pivot point? (Try to set the pivot points of your meshes to the middle of the mesh if possible for optimal viewing.)\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Should the camera boom offset its Z position to account for a low pivot point? (Try to set the pivot points of your meshes to the middle of the mesh if possible for optimal viewing.)" },
	};
#endif
	void Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset_SetBit(void* Obj)
	{
		((AArtBibleDisplayer*)Obj)->bCameraZOffset = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset = { "bCameraZOffset", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AArtBibleDisplayer), &Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset_SetBit, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_Index_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Index of current asset shown. \n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Index of current asset shown." },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_Index = { "Index", nullptr, (EPropertyFlags)0x0010000000020801, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, Index), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_Index_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_Index_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_NextIndex_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Index to display next (or first if set in defaults.) \n" },
		{ "EditCondition", "!bAutoDisplay" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Index to display next (or first if set in defaults.)" },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_NextIndex = { "NextIndex", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, NextIndex), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_NextIndex_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_NextIndex_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DelayTime_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Time between mesh switches.\n" },
		{ "DisplayName", "Delay" },
		{ "EditCondition", "!bFullRotation" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Time between mesh switches." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DelayTime = { "DelayTime", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, DelayTime), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DelayTime_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DelayTime_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_RotationSpeed_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// How quickly the rotation should happen.\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "How quickly the rotation should happen." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_RotationSpeed = { "RotationSpeed", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, RotationSpeed), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_RotationSpeed_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_RotationSpeed_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Set to true if we should attempt to do a full rotation around the object before we switch to a new mesh. Overrides the delay timer.\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Set to true if we should attempt to do a full rotation around the object before we switch to a new mesh. Overrides the delay timer." },
	};
#endif
	void Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation_SetBit(void* Obj)
	{
		((AArtBibleDisplayer*)Obj)->bFullRotation = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation = { "bFullRotation", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AArtBibleDisplayer), &Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation_SetBit, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Should the camera rotate around the object?\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Should the camera rotate around the object?" },
	};
#endif
	void Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate_SetBit(void* Obj)
	{
		((AArtBibleDisplayer*)Obj)->bRotate = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate = { "bRotate", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AArtBibleDisplayer), &Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate_SetBit, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Should the Art Displayer loop back around after reaching the end of the array?\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Should the Art Displayer loop back around after reaching the end of the array?" },
	};
#endif
	void Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop_SetBit(void* Obj)
	{
		((AArtBibleDisplayer*)Obj)->bLoop = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop = { "bLoop", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AArtBibleDisplayer), &Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop_SetBit, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Should the Art Displayer automatically switch to the next mesh in line?\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Should the Art Displayer automatically switch to the next mesh in line?" },
	};
#endif
	void Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay_SetBit(void* Obj)
	{
		((AArtBibleDisplayer*)Obj)->bAutoDisplay = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay = { "bAutoDisplay", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AArtBibleDisplayer), &Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay_SetBit, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_CameraBoom_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Camera" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_CameraBoom = { "CameraBoom", nullptr, (EPropertyFlags)0x00100000000a001d, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, CameraBoom), Z_Construct_UClass_USpringArmComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_CameraBoom_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_CameraBoom_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Folders to fill from if desired. Leave empty to only add assets manually. Not guaranteed to find all meshes.\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Folders to fill from if desired. Leave empty to only add assets manually. Not guaranteed to find all meshes." },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders = { "AutoFillFolders", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, AutoFillFolders), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders_MetaData)) };
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders_Inner = { "AutoFillFolders", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes_MetaData[] = {
		{ "Category", "Camera" },
		{ "Comment", "// Add static meshes to this array in the order you want them displayed.\n" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
		{ "ToolTip", "Add static meshes to this array in the order you want them displayed." },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes = { "StaticMeshes", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, StaticMeshes), METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes_Inner = { "StaticMeshes", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DisplayedObject_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/ArtBibleDisplayer.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DisplayedObject = { "DisplayedObject", nullptr, (EPropertyFlags)0x0010000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AArtBibleDisplayer, DisplayedObject), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DisplayedObject_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DisplayedObject_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AArtBibleDisplayer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bCameraZOffset,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_Index,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_NextIndex,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DelayTime,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_RotationSpeed,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bFullRotation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bRotate,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bLoop,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_bAutoDisplay,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_CameraBoom,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_AutoFillFolders_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_StaticMeshes_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AArtBibleDisplayer_Statics::NewProp_DisplayedObject,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AArtBibleDisplayer_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AArtBibleDisplayer>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AArtBibleDisplayer_Statics::ClassParams = {
		&AArtBibleDisplayer::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_AArtBibleDisplayer_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AArtBibleDisplayer_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AArtBibleDisplayer_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AArtBibleDisplayer()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AArtBibleDisplayer_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AArtBibleDisplayer, 282283711);
	template<> ARTDISPLAYER_API UClass* StaticClass<AArtBibleDisplayer>()
	{
		return AArtBibleDisplayer::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AArtBibleDisplayer(Z_Construct_UClass_AArtBibleDisplayer, &AArtBibleDisplayer::StaticClass, TEXT("/Script/ArtDisplayer"), TEXT("AArtBibleDisplayer"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AArtBibleDisplayer);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
