// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"


AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (HasAuthority())
    {
        FVector Location = GetActorLocation();
        // FVector GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
        FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
        Location += Direction * Speed * DeltaTime;

        float MoveLength = (GlobalTargetLocation - GlobalStartLocation).Length();
        float MovedLength = (Location - GlobalStartLocation).Length();
        
        if (MoveLength <= MovedLength)
        {
            Location = GlobalTargetLocation;
            FVector TempLocation = GlobalTargetLocation;
            GlobalTargetLocation = GlobalStartLocation;
            GlobalStartLocation = TempLocation;
        }

        SetActorLocation(Location);
        
        

        UE_LOG(LogTemp, Display, TEXT("%s"), *(Direction.ToString()));
    }
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    GlobalStartLocation = GetActorLocation();
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);

    UE_LOG(LogTemp, Display, TEXT("StartLocation, GlobalTargetLocation : %s , %s"), *(GlobalStartLocation.ToString()), *(TargetLocation.ToString()));
    if (HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }
}
