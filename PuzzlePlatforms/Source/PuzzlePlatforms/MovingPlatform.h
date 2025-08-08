// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	void ActiveTrigger();
	void RemoveActiveTrigger();

private:
	void MovePlatform();

	UPROPERTY(EditAnywhere, meta=(MakeEditWidget=true))
	FVector TargetLocalLocation;

	UPROPERTY(EditAnywhere)
	float Speed = 200.f;

	UPROPERTY(EditAnywhere)
	bool IsActive = true;

	FVector StartGlobalLocation;
	FVector TargetGlobalLocation;

	UPROPERTY(EditAnywhere)
	int ActiveTriggers = 1;
};
