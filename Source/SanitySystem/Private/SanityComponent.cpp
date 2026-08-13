// SanityComponent.cpp
// Core sanity state machine implementation — fully config-driven.

#include "SanityComponent.h"
#include "SanityConfig.h"
#include "SanityGameplayTags.h"

USanityComponent::USanityComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // event-driven, no per-frame polling
}

void USanityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Config)
	{
		UE_LOG(LogTemp, Warning, TEXT("SanityComponent on %s has no Config assigned — using fallback thresholds."),
			*GetOwner()->GetName());
	}

	CurrentTier = ResolveTierForValue(CurrentSanity);
}

void USanityComponent::ModifySanity(float Delta, FGameplayTag Reason)
{
	float MinSanity, MaxSanity, Unused1, Unused2, Unused3, Unused4;
	GetThresholds(Unused1, Unused2, Unused3, Unused4, MinSanity, MaxSanity);

	const float OldValue = CurrentSanity;
	const float NewValue = FMath::Clamp(CurrentSanity + Delta, MinSanity, MaxSanity);

	if (FMath::IsNearlyEqual(NewValue, OldValue))
	{
		return; // no-op guard: don't broadcast when clamping absorbed the delta entirely
	}

	CurrentSanity = NewValue;
	OnSanityChanged.Broadcast(CurrentSanity, CurrentSanity - OldValue);
	EvaluateTier();
}

void USanityComponent::SetSanity(float NewValue)
{
	float MinSanity, MaxSanity, Unused1, Unused2, Unused3, Unused4;
	GetThresholds(Unused1, Unused2, Unused3, Unused4, MinSanity, MaxSanity);

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
	float Threshold_Uneasy, Threshold_Disturbed, Threshold_Breaking, Threshold_Broken, MinSanity, MaxSanity;
	GetThresholds(Threshold_Uneasy, Threshold_Disturbed, Threshold_Breaking, Threshold_Broken, MinSanity, MaxSanity);

	// Hysteresis: when improving (moving toward Stable), require clearing the
	// threshold by TierHysteresisMargin. When declining, the raw threshold applies
	// immediately. This prevents rapid tier flicker when sanity hovers near a boundary.
	const bool bImproving = Value > CurrentSanity || CurrentTier == FGameplayTag();

	const float EffectiveUneasy = bImproving ? Threshold_Uneasy + TierHysteresisMargin : Threshold_Uneasy;
	const float EffectiveDisturbed = bImproving ? Threshold_Disturbed + TierHysteresisMargin : Threshold_Disturbed;
	const float EffectiveBreaking = bImproving ? Threshold_Breaking + TierHysteresisMargin : Threshold_Breaking;

	// Tier order: Stable (100–Uneasy), Uneasy (Uneasy–Disturbed), Disturbed (Disturbed–Breaking), Breaking (Breaking–0)
	if (Value <= EffectiveBreaking)  return SanityTags::TAG_Sanity_Breaking;
	if (Value <= EffectiveDisturbed) return SanityTags::TAG_Sanity_Critical;
	if (Value <= EffectiveUneasy)    return SanityTags::TAG_Sanity_Uneasy;
	return SanityTags::TAG_Sanity_Stable;
}

void USanityComponent::GetThresholds(
	float& OutUneasy, float& OutDisturbed, float& OutBreaking, float& OutBroken, float& OutMin, float& OutMax) const
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
		// Fallback defaults matching the original config asset defaults
		OutUneasy = 75.f;
		OutDisturbed = 50.f;
		OutBreaking = 25.f;
		OutBroken = 0.f;
		OutMin = 0.f;
		OutMax = 100.f;
	}
}
