#include "SanityRestoreZone.h"
#include "SanityComponent.h"
#include "Components/BoxComponent.h"

ASanityRestoreZone::ASanityRestoreZone()
{
	PrimaryActorTick.bCanEverTick = true;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

void ASanityRestoreZone::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASanityRestoreZone::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ASanityRestoreZone::OnOverlapEnd);
}

void ASanityRestoreZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ActiveSanityComponent.IsValid())
	{
		ActiveSanityComponent->ModifySanity(RestoreRatePerSecond * DeltaTime, FGameplayTag());
	}
}

void ASanityRestoreZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (USanityComponent* SanityComponent = OtherActor->FindComponentByClass<USanityComponent>())
		{
			ActiveSanityComponent = SanityComponent;
		}
	}
}

void ASanityRestoreZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ActiveSanityComponent.IsValid() && ActiveSanityComponent->GetOwner() == OtherActor)
	{
		ActiveSanityComponent.Reset();
	}
}
