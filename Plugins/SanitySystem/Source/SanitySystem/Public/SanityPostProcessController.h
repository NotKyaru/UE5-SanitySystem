#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityPostProcessController.generated.h"

class USanityComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Horror), meta = (BlueprintSpawnableComponent), DisplayName = "Sanity PostProcess Controller")
class SANITYSYSTEM_API USanityPostProcessController : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityPostProcessController();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInterface> SanityMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.1"))
	float InterpSpeed = 3.f;

private:
	UPROPERTY()
	TObjectPtr<USanityComponent> SanityComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SanityMaterialDynamic;

	float TargetVignette = 0.f;
	float TargetChromatic = 0.f;
	float TargetDesaturation = 0.f;
	float TargetGrain = 0.f;
	float CurrentVignette = 0.f;
	float CurrentChromatic = 0.f;
	float CurrentDesaturation = 0.f;
	float CurrentGrain = 0.f;

	UFUNCTION()
	void OnSanityTierChanged(FGameplayTag OldTier, FGameplayTag NewTier);

	void UpdateTargetParametersForTier(FGameplayTag Tier);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
