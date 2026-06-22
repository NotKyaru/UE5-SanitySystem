#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityPostProcessController.generated.h"

class USanityComponent;
class UPostProcessComponent;

/**
 * USanityPostProcessController
 *
 * Attach to the same actor as USanityComponent.
 * Listens to OnSanityChanged and drives scalar parameters
 * on a Post-Process Material in real time.
 *
 * Material Parameter Targets (set these names in your PP Material):
 *   - "SanityNormalized"     : float 0-1, master driver
 *   - "VignetteIntensity"    : float, ramps 0→1 below Threshold_Uneasy
 *   - "ChromaticAberration"  : float, ramps 0→1 below Threshold_Disturbed
 *   - "DesaturationAmount"   : float, ramps 0→1 below Threshold_Breaking
 *   - "NoiseGrainIntensity"  : float, ramps 0→1 below Threshold_Breaking
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent), DisplayName="Sanity PostProcess Controller")
class SANITYSYSTEM_API USanityPostProcessController : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityPostProcessController();

protected:
	virtual void BeginPlay() override;

public:

	/** The post-process material instance to drive. Assign in Blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInstanceDynamic> SanityMaterial;

	/**
	 * How fast the PP parameters interpolate toward target values.
	 * Higher = snappier transitions. Lower = dreamlike fade.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.1"))
	float InterpSpeed = 3.f;

private:
	UPROPERTY()
	TObjectPtr<USanityComponent> SanityComponent;

	// Target values (set on threshold events)
	float TargetVignette = 0.f;
	float TargetChromatic = 0.f;
	float TargetDesaturation = 0.f;
	float TargetGrain = 0.f;

	// Current interpolated values
	float CurrentVignette = 0.f;
	float CurrentChromatic = 0.f;
	float CurrentDesaturation = 0.f;
	float CurrentGrain = 0.f;

	UFUNCTION()
	void OnSanityChanged(float NewValue, float Delta);

	void UpdateTargetParameters(float SanityNormalized);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
