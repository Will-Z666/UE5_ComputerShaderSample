// Copyright Epic Games, Inc. All Rights Reserved.

#include "OoOComputeShader.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOoOComputeShaderModule"

void FOoOComputeShaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	const FString ShaderDirectory = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("OoOComputeShader"))->GetBaseDir(), TEXT("Shaders/Private"));
	AddShaderSourceDirectoryMapping("/OoOShaders", ShaderDirectory);
}

void FOoOComputeShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOoOComputeShaderModule, OoOComputeShader)