#include "SanityEnemyProximity.h"
#include "SanityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

USanityEnemyProximity::USanityEnemyProximity()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USanityEnemyProximity::BeginPlay()
{
	Super::BeginPlay();

	// Cache the player actor once at BeginPlay
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerActor = Player;
	}
}

void USanityEnemyProximity::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerActor.IsValid()) return;

	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), PlayerActor->GetActorLocation());
	if (Distance >= MaxRadius) return;

	if (bRequireLineOfSight && !HasLineOfSight()) return;

	USanityComponent* SC = PlayerActor->FindComponentByClass<USanityComponent>();
	if (!SC) return;

	// Linear falloff: full drain at 0, zero drain at MaxRadius
	const float Alpha = 1.f - (Distance / MaxRadius);
	const float Drain = DrainRatePerSecond * Alpha * DeltaTime;
	SC->ModifySanity(-Drain, SourceTag);
}

bool USanityEnemyProximity::HasLineOfSight() const
{
	if (!PlayerActor.IsValid()) return false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(PlayerActor.Get());

	const FVector Start = GetOwner()->GetActorLocation() + FVector(0, 0, 60.f);
	const FVector End = PlayerActor->GetActorLocation() + FVector(0, 0, 60.f);

	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	return !bBlocked;
}
