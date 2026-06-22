#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SanityConfig.generated.h"

/**
 * USanityConfig
 *
 * DataAsset containing all designer-tunable thresholds and rates.
 * Create one instance in the Content Browser and assign it to the
 * SanityComponent on your PlayerCharacter BP.
 *
 * No recompile needed when tweaking values.
 */
UCLASS(BlueprintType)
class SANITYSYSTEM_API USanityConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	// ----------------------------------------------------------------
	// Sanity Thresholds
	// These values define the four horror stages.
	// Thresholds broadcast OnSanityThresholdReached when crossed.
	// ----------------------------------------------------------------

	/** Stage 1: Unease begins. Subtle vignette. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Uneasy = 75.f;

	/** Stage 2: Disturbed. Chromatic aberration onset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Disturbed = 50.f;

	/** Stage 3: Breaking. Desaturation + grain. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Breaking = 25.f;

	/** Stage 4: Broken. Full visual breakdown, hallucinations. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Broken = 0.f;

	// ----------------------------------------------------------------
	// Sanity Clamp
	// ----------------------------------------------------------------

	/** Minimum sanity (default 0 — broken state). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clamp")
	float MinSanity = 0.f;

	/** Maximum sanity (default 100 — fully sane). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clamp")
	float MaxSanity = 100.f;
};
