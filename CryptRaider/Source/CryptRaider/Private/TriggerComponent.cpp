// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Actor = GetAcceptableActor();
	if (nullptr != Actor)
	{
		//UE_LOG(LogTemp, Display, TEXT("Unlocking"));
		bool IsGrabbed = Actor->ActorHasTag("Grabbed");
		if (nullptr != Mover && false == IsGrabbed)
		{
			UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
			if (nullptr != Component)
			{
				Component->SetSimulatePhysics(false);
				Component->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
			}
			Mover->SetShouldMove(true);
		}
	}
	else
	{
		if (nullptr != Mover)
		{
			Mover->SetShouldMove(false);
		}
	}
}

// it returns only the first Actor it finds.
AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);

	for (AActor* Actor : Actors)
	{
		bool HasAcceptabletag = Actor->ActorHasTag(UnlockTagName);
		if (HasAcceptabletag)
		{
			return Actor;
		}
	}

	return nullptr;
}

void UTriggerComponent::SetMover(UMover* NewMover)
{
	Mover = NewMover;
}