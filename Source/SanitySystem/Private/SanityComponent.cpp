// SanityComponent.cpp
// Core sanity state machine implementation.

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
		UE_LOG(LogTemp, Log, TEXT("SanityComponent on %s has no Config assigned. Using component-level thresholds."),
			*GetOwner()->GetName());
	}

	CurrentTier = ResolveTierForValue(CurrentSanity);
}

void USanityComponent::ModifySanity(float Delta, FGameplayTag Reason)
{
	const float OldValue = CurrentSanity;
	const float NewValue = FMath::Clamp(CurrentSanity + Delta, 0.f, 100.f);

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
	const float OldValue = CurrentSanity;
	const float ClampedValue = FMath::Clamp(NewValue, 0.f, 100.f);

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
	// Hysteresis: when improving (moving toward Stable), require clearing the
	// threshold by TierHysteresisMargin. When declining, the raw threshold applies
	// immediately. This prevents rapid tier flicker when sanity hovers near a boundary.
	const bool bImproving = Value > CurrentSanity || CurrentTier == FGameplayTag();

	const float EffectiveUneasy = bImproving ? UneasyThreshold + TierHysteresisMargin : UneasyThreshold;
	const float EffectiveCritical = bImproving ? CriticalThreshold + TierHysteresisMargin : CriticalThreshold;
	const float EffectiveBreaking = bImproving ? BreakingThreshold + TierHysteresisMargin : BreakingThreshold;

	if (Value <= EffectiveBreaking)  return SanityTags::TAG_Sanity_Breaking;
	if (Value <= EffectiveCritical)  return SanityTags::TAG_Sanity_Critical;
	if (Value <= EffectiveUneasy)    return SanityTags::TAG_Sanity_Uneasy;
	return SanityTags::TAG_Sanity_Stable;
}
