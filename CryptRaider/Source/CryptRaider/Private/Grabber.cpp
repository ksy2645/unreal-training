// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	
	if (UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle())
	{
		UE_LOG(LogTemp, Display, TEXT("PhysicsHandle Name: %s"), *(PhysicsHandle->GetName()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle is nullptr"));
	}
}


// Called every frame	
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (nullptr == PhysicsHandle)
	{
		return;
	}
	
	if (UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + (GetForwardVector() * HoldDistance);
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (nullptr == PhysicsHandle)
	{
		return;
	}
	
	FHitResult HitResult;
	const bool HasHit = GetGrabbableInreach(HitResult);
	if (HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		SetGrabbedActor(HitResult.GetActor());
		GrabbedActor->Tags.Add(GRABBED_FName);
		GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);

	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (nullptr == PhysicsHandle)
	{
		return;
	}

	if (UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent())
	{
		GrabbedComponent->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
		if (GrabbedActor && GrabbedActor->ActorHasTag(GRABBED_FName))
		{
			GrabbedActor->Tags.Remove(GRABBED_FName);
		}
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle(void) const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (nullptr == Result)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}

	return Result;
}

const bool UGrabber::GetGrabbableInreach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + (MaxGrabDistance * GetForwardVector());
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false);
	
	DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Blue, false, 3);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	bool rv = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
	);

	return rv;
}

void UGrabber::SetGrabbedActor(AActor* NewActor)
{
	GrabbedActor = NewActor;
}