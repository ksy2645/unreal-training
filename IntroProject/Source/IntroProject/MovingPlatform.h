// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class INTROPROJECT_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	float Speed = 20.f;

	UPROPERTY(EditAnywhere)
	float MoveDistance;

	UPROPERTY(EditAnywhere, Meta=(MakeEditWidget=true))
	FVector TargetLocation;

	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;
};
