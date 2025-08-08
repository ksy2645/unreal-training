// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
//#include "InputAction.h"


ATank::ATank()
{
    //Super();

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

//    static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT(TEXT(""));
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
    TankPlayerController = GetController<APlayerController>();
    if(TankPlayerController)
    {
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(TankPlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(TankPlayerController)
    {
        FHitResult HitResult;
        TankPlayerController->GetHitResultUnderCursor(
            ECollisionChannel::ECC_Visibility,
            false,
            HitResult);
   
        DrawDebugSphere(
            GetWorld(),
            HitResult.ImpactPoint,
            15,
            12,
            FColor::Red,
            false,
            -1.0f
        );
        
        RotateTurret(HitResult.ImpactPoint, DeltaTime);
    }
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    UE_LOG(LogTemp, Display, TEXT("SetupPlayerInputComponent"));
    if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        Input->BindAction(MoveForward, ETriggerEvent::Triggered, this, &ATank::MoveForwardInput);
        Input->BindAction(IAFire, ETriggerEvent::Started, this, &ATank::FireInput);
        Input->BindAction(Turn, ETriggerEvent::Triggered, this, &ATank::TurnInput);
        //Input->BindAction(IARotateTurret, ETriggerEvent::Triggered, this, &ATank::RotateTurretInput);
    }
}

void ATank::HandleDestruction()
{
    Super::HandleDestruction();
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    bAlive = false;
}

APlayerController* ATank::GetPlayerController() const
{
    return TankPlayerController;
}

void ATank::MoveForwardInput(const FInputActionInstance& Instance)
{
    float Value = Instance.GetValue().Get<float>();
    FVector DeltaLocation(0.f);
    double DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    DeltaLocation.X = Value * MoveSpeed * DeltaTime;
    AddActorLocalOffset(DeltaLocation, true);
}

void ATank::FireInput(const FInputActionInstance& Instance)
{
    Fire();
}

void ATank::TurnInput(const FInputActionInstance& Instance)
{
    float Value = Instance.GetValue().Get<float>();
    FRotator DeltaRotation = FRotator::ZeroRotator;
    DeltaRotation.Yaw = TurretTurnRate * Value * UGameplayStatics::GetWorldDeltaSeconds(this);
    AddActorLocalRotation(DeltaRotation, true);
}

void ATank::RotateTurretInput(const FInputActionInstance& Instance)
{
    float Value = Instance.GetValue().Get<float>();
    UE_LOG(LogTemp, Display, TEXT("RotateTurretInput Value: %f"), Value);
}

