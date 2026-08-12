// SanityComponent.h
// Core sanity state machine. Event-driven (no tick), gameplay-tag tiers, hysteresis-protected.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SanityComponent.generated.h"

class USanityConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChanged, float, NewValue, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityTierChanged, FGameplayTag, OldTier, FGameplayTag, NewTier);

UCLASS(ClassGroup = (Sanity), meta = (BlueprintSpawnableComponent))
class SANITYSYSTEM_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityComponent();

	/** Optional config asset for future non-threshold tuning (e.g. audio cues, VFX intensity curves). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sanity")
	TObjectPtr<USanityConfig> Config;

	/** Sanity value at/below which tier becomes Uneasy. Editable per-instance until migrated into Config. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Thresholds")
	float UneasyThreshold = 70.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Thresholds")
	float CriticalThreshold = 35.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Thresholds")
	float BreakingThreshold = 10.f;

	/** Margin sanity must recover past a threshold before the tier upgrades back, preventing flicker at boundaries. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Thresholds")
	float TierHysteresisMargin = 5.f;

	/** Fires on every actual sanity value change (UI, HUD bars). Skipped if the value doesn't move. */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityChanged OnSanityChanged;

	/** Fires ONLY when the sanity tier actually flips (post-process, audio, AI hooks). */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityTierChanged OnSanityTierChanged;

	/**
	 * The single entry point for all sanity mutation. Every drain/restore/proximity
	 * source should call this instead of touching sanity directly.
	 * @param Delta   Positive to restore, negative to drain.
	 * @param Reason  Gameplay tag identifying the source (e.g. Sanity.Source.EnemyProximity).
	 */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void ModifySanity(float Delta, FGameplayTag Reason);

	/** Hard override, bypassing incremental delta logic (scripted scare moments). */
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void SetSanity(float NewValue);

	UFUNCTION(BlueprintPure, Category = "Sanity")
	float GetCurrentSanity() const { return CurrentSanity; }

	UFUNCTION(BlueprintPure, Category = "Sanity")
	float GetSanityPercent() const { return CurrentSanity / 100.f; }

	UFUNCTION(BlueprintPure, Category = "Sanity")
	FGameplayTag GetCurrentTier() const { return CurrentTier; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Sanity", meta = (AllowPrivateAccess = "true"))
	float CurrentSanity = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Sanity", meta = (AllowPrivateAccess = "true"))
	FGameplayTag CurrentTier;

private:
	/** Re-evaluates CurrentTier against thresholds with hysteresis; broadcasts only on actual change. */
	void EvaluateTier();

	/** Resolves the tier for a given value, applying hysteresis relative to the current tier. */
	FGameplayTag ResolveTierForValue(float Value) const;
};
