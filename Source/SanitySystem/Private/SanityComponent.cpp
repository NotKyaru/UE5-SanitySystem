#include "SanityComponent.h"
#include "SanityConfig.h"

USanityComponent::USanityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USanityComponent::BeginPlay()
{
	Super::BeginPlay();
	SanityValue = InitialSanity;
}

void USanityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USanityComponent::ModifySanity(float Delta, FName Source)
{
	if (FMath::IsNearlyZero(Delta)) return;

	const float PreviousValue = SanityValue;

	const float MinVal = SanityConfig ? SanityConfig->MinSanity : 0.f;
	const float MaxVal = SanityConfig ? SanityConfig->MaxSanity : 100.f;
	SanityValue = FMath::Clamp(SanityValue + Delta, MinVal, MaxVal);

	OnSanityChanged.Broadcast(SanityValue, Delta);
	CheckThresholds(PreviousValue, Source);
}

void USanityComponent::SetSanity(float NewValue)
{
	const float MinVal = SanityConfig ? SanityConfig->MinSanity : 0.f;
	const float MaxVal = SanityConfig ? SanityConfig->MaxSanity : 100.f;
	SanityValue = FMath::Clamp(NewValue, MinVal, MaxVal);
	TriggeredThresholds.Empty();
	OnSanityChanged.Broadcast(SanityValue, 0.f);
}

void USanityComponent::CheckThresholds(float PreviousValue, FName Source)
{
	if (!SanityConfig) return;

	TArray<float> Thresholds = {
		SanityConfig->Threshold_Uneasy,
		SanityConfig->Threshold_Disturbed,
		SanityConfig->Threshold_Breaking,
		SanityConfig->Threshold_Broken
	};

	for (float Threshold : Thresholds)
	{
		// Crossed downward and hasn't fired yet
		if (PreviousValue > Threshold && SanityValue <= Threshold
			&& !TriggeredThresholds.Contains(Threshold))
		{
			TriggeredThresholds.Add(Threshold);
			OnSanityThresholdReached.Broadcast(Threshold, Source);
		}
		// Recovered above threshold — allow it to fire again if fallen below later
		else if (SanityValue > Threshold && TriggeredThresholds.Contains(Threshold))
		{
			TriggeredThresholds.Remove(Threshold);
		}
	}
}
