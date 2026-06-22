#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanityDrainVolume.generated.h"

class UBoxComponent;
class USanityComponent;

/**
 * ASanityDrainVolume
 *
 * Place this actor in the level to create a zone that drains
 * the player's sanity on overlap (darkness pockets, cursed areas,
 * ritual sites, etc.).
 *
 * Drain is applied per second while the player remains inside.
 */
UCLASS()
class SANITYSYSTEM_API ASanityDrainVolume : public AActor
{
	GENERATED_BODY()

public:
	ASanityDrainVolume();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ----------------------------------------------------------------
	// Configuration
	// ----------------------------------------------------------------

	/** How much sanity to drain per second while inside. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Drain", meta = (ClampMin = "0.0"))
	float DrainRatePerSecond = 5.f;

	/** Tag used to identify this drain source in logs and delegates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Drain")
	FName SourceTag = FName("DarkZone");

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerVolume;

	/** Cached reference to the overlapping player's SanityComponent. Null when no player is inside. */
	TWeakObjectPtr<USanityComponent> ActiveSanityComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
