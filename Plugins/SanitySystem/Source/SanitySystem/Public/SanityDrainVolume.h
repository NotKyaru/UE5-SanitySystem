#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SanityDrainVolume.generated.h"

class UBoxComponent;
class USanityComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Drain", meta = (ClampMin = "0.0"))
	float DrainRatePerSecond = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Drain")
	FName SourceTag = FName("DarkZone");

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
