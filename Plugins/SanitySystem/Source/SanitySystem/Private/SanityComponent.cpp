#include "SanityComponent.h"
#include "SanityConfig.h"
#include "SanityGameplayTags.h"

USanityComponent::USanityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USanityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Config)
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityComponent on %s has no Config assigned; using fallback thresholds."), *GetOwner()->GetName());
	}

	CurrentSanity = FMath::Clamp(CurrentSanity, Config ? Config->MinSanity : 0.f, Config ? Config->MaxSanity : 100.f);
	CurrentTier = ResolveTierForValue(CurrentSanity);
}

void USanityComponent::ModifySanity(float Delta, FGameplayTag Reason)
{
	float Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity;
	GetThresholds(Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity);

	const float OldValue = CurrentSanity;
	const float NewValue = FMath::Clamp(CurrentSanity + Delta, MinSanity, MaxSanity);
	if (FMath::IsNearlyEqual(NewValue, OldValue))
	{
		return;
	}

	CurrentSanity = NewValue;
	OnSanityChanged.Broadcast(CurrentSanity, CurrentSanity - OldValue);
	EvaluateTier();
}

void USanityComponent::SetSanity(float NewValue)
{
	float Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity;
	GetThresholds(Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity);

	const float OldValue = CurrentSanity;
	const float ClampedValue = FMath::Clamp(NewValue, MinSanity, MaxSanity);
	if (FMath::IsNearlyEqual(ClampedValue, OldValue))
	{
		return;
	}

	CurrentSanity = ClampedValue;
	OnSanityChanged.Broadcast(CurrentSanity, CurrentSanity - OldValue);
	EvaluateTier();
}

void USanityComponent::EvaluateTier()
{
	const FGameplayTag NewTier = ResolveTierForValue(CurrentSanity);
	if (NewTier != CurrentTier)
	{
		const FGameplayTag OldTier = CurrentTier;
		CurrentTier = NewTier;
		OnSanityTierChanged.Broadcast(OldTier, NewTier);
	}
}

FGameplayTag USanityComponent::ResolveTierForValue(float Value) const
{
	float Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity;
	GetThresholds(Uneasy, Disturbed, Breaking, Broken, MinSanity, MaxSanity);

	const bool bImproving = Value > CurrentSanity || !CurrentTier.IsValid();
	const float EffectiveUneasy = bImproving ? Uneasy + TierHysteresisMargin : Uneasy;
	const float EffectiveDisturbed = bImproving ? Disturbed + TierHysteresisMargin : Disturbed;
	const float EffectiveBreaking = bImproving ? Breaking + TierHysteresisMargin : Breaking;

	if (Value <= EffectiveBreaking) return TAG_Sanity_Breaking;
	if (Value <= EffectiveDisturbed) return TAG_Sanity_Critical;
	if (Value <= EffectiveUneasy) return TAG_Sanity_Uneasy;
	return TAG_Sanity_Stable;
}

void USanityComponent::GetThresholds(float& OutUneasy, float& OutDisturbed, float& OutBreaking, float& OutBroken, float& OutMin, float& OutMax) const
{
	if (Config)
	{
		OutUneasy = Config->Threshold_Uneasy;
		OutDisturbed = Config->Threshold_Disturbed;
		OutBreaking = Config->Threshold_Breaking;
		OutBroken = Config->Threshold_Broken;
		OutMin = Config->MinSanity;
		OutMax = Config->MaxSanity;
	}
	else
	{
		OutUneasy = 75.f;
		OutDisturbed = 50.f;
		OutBreaking = 25.f;
		OutBroken = 0.f;
		OutMin = 0.f;
		OutMax = 100.f;
	}
}
