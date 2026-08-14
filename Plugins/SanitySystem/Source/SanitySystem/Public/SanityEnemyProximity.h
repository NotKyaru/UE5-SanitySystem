#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityEnemyProximity.generated.h"

class USanityComponent;
class ACharacter;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity", meta = (ClampMin = "0.0"))
	float MaxRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity", meta = (ClampMin = "0.0"))
	float DrainRatePerSecond = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity")
	FName SourceTag = FName("EnemyProximity");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity|Proximity")
	bool bRequireLineOfSight = false;

private:
	TWeakObjectPtr<ACharacter> PlayerActor;
	bool HasLineOfSight() const;
};
