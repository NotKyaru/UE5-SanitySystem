#include "SanityPostProcessController.h"
#include "SanityComponent.h"
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
		SanityComponent->OnSanityChanged.AddDynamic(this, &USanityPostProcessController::OnSanityChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityPostProcessController: No USanityComponent found on %s"), *GetOwner()->GetName());
	}
}

void USanityPostProcessController::OnSanityChanged(float NewValue, float Delta)
{
	const float Normalized = NewValue / 100.f;
	UpdateTargetParameters(Normalized);

	if (SanityMaterial)
	{
		SanityMaterial->SetScalarParameterValue(FName("SanityNormalized"), Normalized);
	}
}

void USanityPostProcessController::UpdateTargetParameters(float SanityNormalized)
{
	// Vignette: ramps in below 0.75 (Threshold_Uneasy)
	TargetVignette = FMath::GetMappedRangeValueClamped(
		FVector2D(0.75f, 0.5f), FVector2D(0.f, 1.f), SanityNormalized);

	// Chromatic Aberration: ramps in below 0.5 (Threshold_Disturbed)
	TargetChromatic = FMath::GetMappedRangeValueClamped(
		FVector2D(0.5f, 0.25f), FVector2D(0.f, 1.f), SanityNormalized);

	// Desaturation + Grain: ramp in below 0.25 (Threshold_Breaking)
	TargetDesaturation = FMath::GetMappedRangeValueClamped(
		FVector2D(0.25f, 0.f), FVector2D(0.f, 1.f), SanityNormalized);

	TargetGrain = TargetDesaturation;
}

void USanityPostProcessController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SanityMaterial) return;

	// Interpolate current values toward targets for smooth transitions
	CurrentVignette    = FMath::FInterpTo(CurrentVignette,    TargetVignette,    DeltaTime, InterpSpeed);
	CurrentChromatic   = FMath::FInterpTo(CurrentChromatic,   TargetChromatic,   DeltaTime, InterpSpeed);
	CurrentDesaturation = FMath::FInterpTo(CurrentDesaturation, TargetDesaturation, DeltaTime, InterpSpeed);
	CurrentGrain       = FMath::FInterpTo(CurrentGrain,       TargetGrain,       DeltaTime, InterpSpeed);

	SanityMaterial->SetScalarParameterValue(FName("VignetteIntensity"),   CurrentVignette);
	SanityMaterial->SetScalarParameterValue(FName("ChromaticAberration"), CurrentChromatic);
	SanityMaterial->SetScalarParameterValue(FName("DesaturationAmount"),  CurrentDesaturation);
	SanityMaterial->SetScalarParameterValue(FName("NoiseGrainIntensity"), CurrentGrain);
}
