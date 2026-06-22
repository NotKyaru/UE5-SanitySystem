#include "SanityDrainVolume.h"
#include "SanityComponent.h"
#include "Components/BoxComponent.h"

ASanityDrainVolume::ASanityDrainVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

void ASanityDrainVolume::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASanityDrainVolume::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ASanityDrainVolume::OnOverlapEnd);
}

void ASanityDrainVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveSanityComponent.IsValid())
	{
		ActiveSanityComponent->ModifySanity(-DrainRatePerSecond * DeltaTime, SourceTag);
	}
}

void ASanityDrainVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (USanityComponent* SC = OtherActor->FindComponentByClass<USanityComponent>())
	{
		ActiveSanityComponent = SC;
	}
}

void ASanityDrainVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ActiveSanityComponent.IsValid() &&
		ActiveSanityComponent->GetOwner() == OtherActor)
	{
		ActiveSanityComponent.Reset();
	}
}
