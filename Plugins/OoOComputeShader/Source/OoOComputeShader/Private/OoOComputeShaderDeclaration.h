#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/TextureRenderTarget2D.h"

#include "OoOComputeShaderDeclaration.generated.h"

//This struct act as a container for all the parameters that the client needs to pass to the Compute Shader Manager.
struct OOOCOMPUTESHADER_API FOoOCSParameters
{
	int X;
	int Y;
	int Z;

	
	UTexture2D* InputTexture;
	float Seed;

	FRenderTarget* RenderTarget;

	FOoOCSParameters(int x, int y, int z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};

// This is a public interface that we define so outside code can invoke our compute shader.
class OOOCOMPUTESHADER_API FOoOComputeShaderInterface
{
public:
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FOoOCSParameters Params
	);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FOoOCSParameters Params
	)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[Params](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(RHICmdList, Params);
			});
	}

	// Dispatches this shader. Can be called from any thread
	static void Dispatch(
		FOoOCSParameters Params
	)
	{
		if (IsInRenderingThread()) {
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params);
		}
		else {
			DispatchGameThread(Params);
		}
	}

};



UCLASS() // Change the _API to match your project
class OOOCOMPUTESHADER_API UOoOComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	// Execute the actual load
	virtual void Activate() override {
		//InputTexture->SetForceMipLevelsToBeResident(30.0f);
		//InputTexture->WaitForStreaming();

		// Create a dispatch parameters struct and set our desired seed
		FOoOCSParameters Params(RT->SizeX, RT->SizeY, 1);
		Params.RenderTarget = RT->GameThread_GetRenderTargetResource();
		Params.Seed = Seed;
		Params.InputTexture = InputTexture;

		// Dispatch the compute shader and wait until it completes
		FOoOComputeShaderInterface::Dispatch(Params);
	}

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UOoOComputeShaderLibrary_AsyncExecution* ExecuteMaterialRTComputeShader(UObject* WorldContextObject, UTextureRenderTarget2D* RT, UTexture2D* InputTexture, float Seed) {
		UOoOComputeShaderLibrary_AsyncExecution* Action = NewObject<UOoOComputeShaderLibrary_AsyncExecution>();
		Action->RT = RT;
		Action->InputTexture = InputTexture;
		Action->Seed = Seed;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}
	UTextureRenderTarget2D* RT;
	
	UTexture2D* InputTexture;
	float Seed;
};