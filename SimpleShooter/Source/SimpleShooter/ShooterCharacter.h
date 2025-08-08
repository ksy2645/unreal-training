// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class UInputAction;
class AGun;
struct FInputActionInstance;

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void PullTrigger();

protected:
	UPROPERTY(EditAnywhere, Category = "Inputs")
	class UInputMappingContext* IMCWraithController;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IAMoveForward;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IAMoveRight;
	
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IALookUp;
	
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IALookRight;
	
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IAJump;
	
	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* IAFire;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	float MoveSpeedScale = 1.f;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	float MouseSensitive = 15.f;

	void MoveForwardIMP(const FInputActionInstance& Instance);
	void MoveRightIMP(const FInputActionInstance& Instance);
	void LookUpIMP(const FInputActionInstance& Instance);
	void LookRightIMP(const FInputActionInstance& Instance);
	void JumpIMP(const FInputActionInstance& Instance);
	void FireIMP();

	void MoveXY();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	AGun* Gun;

	APlayerController* WraithPlayerController;
	FVector MoveDirection = FVector(0, 0, 0);
};
