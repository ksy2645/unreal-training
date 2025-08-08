// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"

#include "Kismet/GameplayStatics.h"

ATower::ATower()
{

}

void ATower::BeginPlay()
{
    Super::BeginPlay();

    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
    GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Find the distance to the Tank
    if(IsInFireRange())
    {
        // If in range, rotate turret toward Tank
        RotateTurret(Tank->GetActorLocation(), DeltaTime);
    }
}

void ATower::HandleDestruction()
{
    Super::HandleDestruction();
    Destroy();
}

void ATower::CheckFireCondition()
{
    if (nullptr == Tank)
        return;


    if(IsInFireRange() && Tank->bAlive)
    {
        //UE_LOG(LogTemp, Display, TEXT("CheckFireCondition"));
        Fire();
    }
}

bool ATower::IsInFireRange() const
{
    bool rv = false;
    if(Tank)
    {
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
        // Check to see if the Tank is in range
        rv = Distance <= FireRange;
    }

    return rv;
}