// SanityPostProcessController.cpp
// Drives post-process material based on sanity tier changes.

#include "SanityPostProcessController.h"
#include "SanityComponent.h"
#include "SanityGameplayTags.h"
#include "Materials/MaterialInstanceDynamic.h"

USanityPostProcessController::USanityPostProcessController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USanityPostProcessController::BeginPlay()
{
	Super::BeginPlay();

	SanityComponent = GetOwner()->FindComponentByClass<USanityComponent>();
	if (SanityComponent)
	{
		SanityComponent->OnSanityTierChanged.AddDynamic(this, &USanityPostProcessController::OnSanityTierChanged);
		// Initialize to current tier on startup
		UpdateTargetParametersForTier(SanityComponent->GetCurrentTier());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityPostProcessController: No USanityComponent found on %s"), *GetOwner()->GetName());
	}
}

void USanityPostProcessController::OnSanityTierChanged(FGameplayTag OldTier, FGameplayTag NewTier)
{
	UpdateTargetParametersForTier(NewTier);
}

void USanityPostProcessController::UpdateTargetParametersForTier(FGameplayTag Tier)
{
	// Tier-driven target values. Adjust these to match your horror intensity curve.
	if (Tier == SanityTags::TAG_Sanity_Stable)
	{
		TargetVignette = 0.f;
		TargetChromatic = 0.f;
		TargetDesaturation = 0.f;
		TargetGrain = 0.f;
	}
	else if (Tier == SanityTags::TAG_Sanity_Uneasy)
	{
		TargetVignette = 0.3f;
		TargetChromatic = 0.f;
		TargetDesaturation = 0.f;
		TargetGrain = 0.f;
	}
	else if (Tier == SanityTags::TAG_Sanity_Critical)
	{
		TargetVignette = 0.6f;
		TargetChromatic = 0.4f;
		TargetDesaturation = 0.2f;
		TargetGrain = 0.2f;
	}
	else if (Tier == SanityTags::TAG_Sanity_Breaking)
	{
		TargetVignette = 1.f;
		TargetChromatic = 0.8f;
		TargetDesaturation = 0.6f;
		TargetGrain = 0.6f;
	}
}

void USanityPostProcessController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SanityMaterial) return;

	// Interpolate current values toward targets for smooth transitions
	CurrentVignette = FMath::FInterpTo(CurrentVignette, TargetVignette, DeltaTime, InterpSpeed);
	CurrentChromatic = FMath::FInterpTo(CurrentChromatic, TargetChromatic, DeltaTime, InterpSpeed);
	CurrentDesaturation = FMath::FInterpTo(CurrentDesaturation, TargetDesaturation, DeltaTime, InterpSpeed);
	CurrentGrain = FMath::FInterpTo(CurrentGrain, TargetGrain, DeltaTime, InterpSpeed);

	SanityMaterial->SetScalarParameterValue(FName("VignetteIntensity"), CurrentVignette);
	SanityMaterial->SetScalarParameterValue(FName("ChromaticAberration"), CurrentChromatic);
	SanityMaterial->SetScalarParameterValue(FName("DesaturationAmount"), CurrentDesaturation);
	SanityMaterial->SetScalarParameterValue(FName("NoiseGrainIntensity"), CurrentGrain);
}
