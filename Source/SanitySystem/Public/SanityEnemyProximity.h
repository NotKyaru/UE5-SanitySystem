#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityEnemyProximity.generated.h"

class USanityComponent;

/**
 * USanityEnemyProximity
 *
 * Attach this component to any enemy Actor.
 * On Tick, it checks distance to the player and drains sanity
 * proportionally — closer enemies drain faster.
 *
 * Drain formula: DrainRate * (1 - Distance / MaxRadius) * DeltaTime
 * At distance 0 (direct contact): full DrainRatePerSecond.
 * At MaxRadius: drain reaches 0.
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent), DisplayName="Sanity Enemy Proximity")
class SANITYSYSTEM_API USanityEnemyProximity : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityEnemyProximity();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ----------------------------------------------------------------
	// Configuration
	// ----------------------------------------------------------------

	/** Maximum distance (cm) at which this enemy affects sanity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity", meta = (ClampMin = "0.0"))
	float MaxRadius = 800.f;

	/** Drain rate at point-blank range (units/sec). Scales linearly with distance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity", meta = (ClampMin = "0.0"))
	float DrainRatePerSecond = 10.f;

	/** Tag for delegate source identification. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity")
	FName SourceTag = FName("EnemyProximity");

	/**
	 * If true, drain only applies when the enemy has line-of-sight to the player.
	 * Uses a simple EyeHeight trace — useful for enemies that are "sensed" rather than seen.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity")
	bool bRequireLineOfSight = false;

private:
	TWeakObjectPtr<AActor> PlayerActor;

	bool HasLineOfSight() const;
};
