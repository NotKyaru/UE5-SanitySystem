#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanityRestoreZone.generated.h"

class UBoxComponent;
class USanityComponent;

/**
 * ASanityRestoreZone
 *
 * Safe room trigger. Slowly restores the player's sanity
 * while they remain inside. Use for save points, lit areas,
 * or narrative "breather" moments.
 */
UCLASS()
class SANITYSYSTEM_API ASanityRestoreZone : public AActor
{
	GENERATED_BODY()

public:
	ASanityRestoreZone();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ----------------------------------------------------------------
	// Configuration
	// ----------------------------------------------------------------

	/** How much sanity to restore per second while inside. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Restore", meta = (ClampMin = "0.0"))
	float RestoreRatePerSecond = 8.f;

	/** Tag for delegate source identification. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Restore")
	FName SourceTag = FName("SafeRoom");

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerVolume;

	TWeakObjectPtr<USanityComponent> ActiveSanityComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
