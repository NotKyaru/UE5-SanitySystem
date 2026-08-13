// SanityPostProcessController.h
// Drives post-process material parameters based on sanity tier changes.
// Attach to the same actor as USanityComponent.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityPostProcessController.generated.h"

class USanityComponent;

UCLASS(ClassGroup = (Horror), meta = (BlueprintSpawnableComponent), DisplayName = "Sanity PostProcess Controller")
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
	TObjectPtr SanityMaterial;

	/** How fast the PP parameters interpolate toward target values. Higher = snappier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.1"))
	float InterpSpeed = 3.f;

private:
	UPROPERTY()
	TObjectPtr SanityComponent;

	// Target values (set on tier events)
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
	void OnSanityTierChanged(FGameplayTag OldTier, FGameplayTag NewTier);

	void UpdateTargetParametersForTier(FGameplayTag Tier);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
