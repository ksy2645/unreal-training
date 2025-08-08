// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "InputActionValue.h"

#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;

struct FInputActionInstance;
/**
 * 
 */
UCLASS()
class TOONTANKS_API ATank : public ABasePawn
{
	GENERATED_BODY()

public:
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleDestruction();

	APlayerController* GetPlayerController() const;

	bool bAlive = true;

protected:
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* MoveForward;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IAFire;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* Turn;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IARotateTurret;
	
	void MoveForwardInput(const FInputActionInstance& Instance);

	void FireInput(const FInputActionInstance& Instance);

	void TurnInput(const FInputActionInstance& Instance);

	void RotateTurretInput(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurretTurnRate = 200.f;

	APlayerController* TankPlayerController;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;
};
