#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SanityConfig.generated.h"

UCLASS(BlueprintType)
class SANITYSYSTEM_API USanityConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Uneasy = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Disturbed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Breaking = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Threshold_Broken = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clamp")
	float MinSanity = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clamp")
	float MaxSanity = 100.f;
};
