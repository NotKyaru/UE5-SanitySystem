// SanityComponent.h
// Core sanity state machine. Event-driven, config-driven thresholds, hysteresis-protected.

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

	/** Config asset holding all thresholds and clamp values. Assign in editor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sanity")
	TObjectPtr<USanityConfig> Config;

	/** Hysteresis margin: sanity must recover past a threshold by this amount before tier upgrades. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sanity|Thresholds")
	float TierHysteresisMargin = 5.f;

	/** Fires on every actual sanity value change (UI, HUD bars). Skipped if the value doesn't move. */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityChanged OnSanityChanged;

	/** Fires ONLY when the sanity tier actually flips (post-process, audio, AI hooks). */
	UPROPERTY(BlueprintAssignable, Category = "Sanity|Events")
	FOnSanityTierChanged OnSanityTierChanged;

	/**
	 * The single entry point for all sanity mutation.
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
	void EvaluateTier();
	FGameplayTag ResolveTierForValue(float Value) const;

	/** Helper to safely read thresholds from Config, with fallback defaults. */
	void GetThresholds(float& OutUneasy, float& OutDisturbed, float& OutBreaking, float& OutBroken, float& OutMin, float& OutMax) const;
};
