#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityConfig.h"
#include "SanityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChanged, float, NewValue, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityThresholdReached, float, Threshold, FName, Source);

/**
 * USanityComponent
 *
 * Core component for the Sanity & Perception system.
 * Attach to APlayerCharacter. Holds SanityValue (0-100) and
 * broadcasts delegates when thresholds are crossed.
 *
 * All drain/restore logic lives in modifier classes (SanityDrainVolume,
 * SanityEnemyProximity, SanityRestoreZone) — this component is purely
 * the source of truth.
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent), DisplayName="Sanity Component")
class SANITYSYSTEM_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ----------------------------------------------------------------
	// Core API
	// ----------------------------------------------------------------

	/**
	 * Modify sanity by Delta (negative = drain, positive = restore).
	 * Source is a tag for debugging/logging (e.g. "EnemyProximity", "DarkZone").
	 */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void ModifySanity(float Delta, FName Source);

	/** Returns current sanity value (0-100). */
	UFUNCTION(BlueprintPure, Category = "Sanity")
	float GetSanity() const { return SanityValue; }

	/** Returns sanity normalized to 0-1 range. Useful for material parameter binding. */
	UFUNCTION(BlueprintPure, Category = "Sanity")
	float GetSanityNormalized() const { return SanityValue / 100.f; }

	/** Force-set sanity (use for saves/loads, not gameplay). */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void SetSanity(float NewValue);

	// ----------------------------------------------------------------
	// Delegates
	// ----------------------------------------------------------------

	/** Fired every time SanityValue changes. */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityChanged OnSanityChanged;

	/** Fired when SanityValue crosses a configured threshold (only fires once per crossing). */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityThresholdReached OnSanityThresholdReached;

	// ----------------------------------------------------------------
	// Configuration
	// ----------------------------------------------------------------

	/** Designer-facing DataAsset for threshold and clamp configuration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sanity|Config")
	TObjectPtr<USanityConfig> SanityConfig;

	/** Starting sanity value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sanity|Config", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float InitialSanity = 100.f;

private:
	float SanityValue;

	/** Tracks which thresholds have already fired to avoid repeat broadcasts. */
	TArray<float> TriggeredThresholds;

	void CheckThresholds(float PreviousValue, FName Source);
};
