#include "SanityEnemyProximity.h"
#include "SanityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

USanityEnemyProximity::USanityEnemyProximity()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USanityEnemyProximity::BeginPlay()
{
	Super::BeginPlay();
	PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void USanityEnemyProximity::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerActor.IsValid() || MaxRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), PlayerActor->GetActorLocation());
	if (Distance >= MaxRadius || (bRequireLineOfSight && !HasLineOfSight()))
	{
		return;
	}

	if (USanityComponent* SanityComponent = PlayerActor->FindComponentByClass<USanityComponent>())
	{
		const float Alpha = 1.f - (Distance / MaxRadius);
		SanityComponent->ModifySanity(-DrainRatePerSecond * Alpha * DeltaTime, FGameplayTag());
	}
}

bool USanityEnemyProximity::HasLineOfSight() const
{
	if (!PlayerActor.IsValid())
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SanityEnemyProximityLOS), false, GetOwner());
	Params.AddIgnoredActor(PlayerActor.Get());

	const FVector Start = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	const FVector End = PlayerActor->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	return !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}
