// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class UNREALLEARNINGKIT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

private:
	// Moving Platform
	UPROPERTY(EditAnyWhere, Category="Moving Platform")
	FVector PlatformVelocity = FVector(100, 0, 0);
	UPROPERTY(EditAnyWhere, Category="Moving Platform")
	float MoveDistance = 100;
	//UPROPERTY(EditAnyWhere, Category="Moving Platform")
	FVector StartLocation;
	// Rotate Platform
	UPROPERTY(EditAnyWhere, Category="Rotate Platform")
	FRotator RotateVelocity = FRotator(0, 100, 0); // roll pitch yaw
	UPROPERTY(EditAnyWhere, Category="Rotate Platform")
	bool IsRotate = false;

	// functions
	void MovePlatform(float DeltaTime);
	void RotatePlatform(float DeltaTime);

	// pure
	bool ShouldPlatformReturn() const;
	float GetDistanceMoved() const;
};
