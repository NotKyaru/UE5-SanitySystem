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
		UpdateTargetParametersForTier(SanityComponent->GetCurrentTier());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityPostProcessController: No USanityComponent found on %s"), *GetOwner()->GetName());
	}

	if (SanityMaterial)
	{
		SanityMaterialDynamic = UMaterialInstanceDynamic::Create(SanityMaterial, this);
		if (!SanityMaterialDynamic)
		{
			UE_LOG(LogTemp, Warning, TEXT("SanityPostProcessController: Failed to create dynamic material instance on %s"), *GetOwner()->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityPostProcessController: No SanityMaterial assigned on %s"), *GetOwner()->GetName());
	}
}

void USanityPostProcessController::OnSanityTierChanged(FGameplayTag OldTier, FGameplayTag NewTier)
{
	UpdateTargetParametersForTier(NewTier);
}

void USanityPostProcessController::UpdateTargetParametersForTier(FGameplayTag Tier)
{
	if (Tier == TAG_Sanity_Stable)
	{
		TargetVignette = 0.f;
		TargetChromatic = 0.f;
		TargetDesaturation = 0.f;
		TargetGrain = 0.f;
	}
	else if (Tier == TAG_Sanity_Uneasy)
	{
		TargetVignette = 0.3f;
		TargetChromatic = 0.f;
		TargetDesaturation = 0.f;
		TargetGrain = 0.f;
	}
	else if (Tier == TAG_Sanity_Critical)
	{
		TargetVignette = 0.6f;
		TargetChromatic = 0.4f;
		TargetDesaturation = 0.2f;
		TargetGrain = 0.2f;
	}
	else if (Tier == TAG_Sanity_Breaking)
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

	if (!SanityMaterialDynamic)
	{
		return;
	}

	CurrentVignette = FMath::FInterpTo(CurrentVignette, TargetVignette, DeltaTime, InterpSpeed);
	CurrentChromatic = FMath::FInterpTo(CurrentChromatic, TargetChromatic, DeltaTime, InterpSpeed);
	CurrentDesaturation = FMath::FInterpTo(CurrentDesaturation, TargetDesaturation, DeltaTime, InterpSpeed);
	CurrentGrain = FMath::FInterpTo(CurrentGrain, TargetGrain, DeltaTime, InterpSpeed);

	SanityMaterialDynamic->SetScalarParameterValue(TEXT("VignetteIntensity"), CurrentVignette);
	SanityMaterialDynamic->SetScalarParameterValue(TEXT("ChromaticAberration"), CurrentChromatic);
	SanityMaterialDynamic->SetScalarParameterValue(TEXT("DesaturationAmount"), CurrentDesaturation);
	SanityMaterialDynamic->SetScalarParameterValue(TEXT("NoiseGrainIntensity"), CurrentGrain);
}
