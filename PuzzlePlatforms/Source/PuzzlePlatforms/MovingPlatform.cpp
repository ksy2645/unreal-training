// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

#include "Kismet/GameplayStatics.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    StartGlobalLocation = GetActorLocation();
    TargetGlobalLocation = GetTransform().TransformPosition(TargetLocalLocation);

    if (HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }
}

void AMovingPlatform::ActiveTrigger()
{
    ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
    if (0 < ActiveTriggers)
        ActiveTriggers--;
}


void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MovePlatform();
}


void AMovingPlatform::MovePlatform()
{
    double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    if (1 <= ActiveTriggers)
    {
        if (HasAuthority())
        {
            FVector Direction = (TargetGlobalLocation - StartGlobalLocation).GetSafeNormal();
            FVector NewLocation = GetActorLocation() + (Direction * Speed * DeltaTime);
    
            float TargetMoveLength = (TargetGlobalLocation - StartGlobalLocation).Length();
            float MoveLength = (GetActorLocation() - StartGlobalLocation).Length();
            if (TargetMoveLength <= MoveLength)
            {
                NewLocation = TargetGlobalLocation;
                FVector Swap = TargetGlobalLocation;
                TargetGlobalLocation = StartGlobalLocation;
                StartGlobalLocation = Swap;
            }
    
            SetActorLocation(NewLocation);
        }
    }
}