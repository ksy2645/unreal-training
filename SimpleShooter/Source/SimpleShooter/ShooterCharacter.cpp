// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameMode.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	WraithPlayerController = GetController<APlayerController>();
	if (WraithPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(WraithPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMCWraithController, 0);
		}
	}

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);

	Health = MaxHealth;
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveXY();
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IAMoveForward, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveForwardIMP);
		Input->BindAction(IAMoveRight, ETriggerEvent::Triggered, this, &AShooterCharacter::MoveRightIMP);
		Input->BindAction(IALookUp, ETriggerEvent::Triggered, this, &AShooterCharacter::LookUpIMP);
		Input->BindAction(IALookRight, ETriggerEvent::Triggered, this, &AShooterCharacter::LookRightIMP);
		Input->BindAction(IAJump, ETriggerEvent::Started, this, &AShooterCharacter::JumpIMP);
		Input->BindAction(IAFire, ETriggerEvent::Started, this, &AShooterCharacter::FireIMP);
	}
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;

	UE_LOG(LogTemp, Display, TEXT("Left Health: %f"), Health);

	if (IsDead())
	{
		if (ASimpleShooterGameMode* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameMode>())
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageApplied;
}

void AShooterCharacter::MoveForwardIMP(const FInputActionInstance& Instance)
{
	//UE_LOG(LogTemp, Display, TEXT("MoveForwardIMP"));
	float Value = Instance.GetValue().Get<float>();
	//double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	MoveDirection += (GetActorForwardVector() * Value);
	//AddMovementInput(GetActorForwardVector() * Value * 0.1);
}

void AShooterCharacter::MoveRightIMP(const FInputActionInstance& Instance)
{
	float Value = Instance.GetValue().Get<float>();
	//double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	MoveDirection += (GetActorRightVector() * Value);
	//AddMovementInput(GetActorRightVector() * Value * 0.1);
}

void AShooterCharacter::LookUpIMP(const FInputActionInstance& Instance)
{
	float Value = Instance.GetValue().Get<float>();
	double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	AddControllerPitchInput(Value * DeltaTime * MouseSensitive);
}

void AShooterCharacter::LookRightIMP(const FInputActionInstance& Instance)
{
	float Value = Instance.GetValue().Get<float>();
	double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	AddControllerYawInput(Value * DeltaTime * MouseSensitive);
}

void AShooterCharacter::JumpIMP(const FInputActionInstance& Instance)
{
	Jump();
}

void AShooterCharacter::FireIMP()
{
	PullTrigger();
}

void AShooterCharacter::MoveXY()
{
	if (true == MoveDirection.IsZero())
		return;

	//double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	MoveDirection.Normalize();
	//UE_LOG(LogTemp, Display, TEXT("%s"), *MoveDirection.ToString());
	MoveDirection *= MoveSpeedScale;
	AddMovementInput(MoveDirection);
	MoveDirection.Set(0, 0, 0);
}

void AShooterCharacter::PullTrigger()
{
	Gun->PullTrigger();
}

