// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = false;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;

}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);
	GameTimerCurrent = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_GameTimer);

	// Move plane forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("%f"), GameTimerCurrent));
	//}
}

void APlayerCharacter::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	CurrentForwardSpeed = 0;
	//float CurrentAcc = -50.0f * Acceleration;
	//float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	//CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_GameTimer, this, &APlayerCharacter::GameTimerExpired, GameTimerStart);
	CurrentForwardSpeed = 1200;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent * InputComponent)
{
	// Check if InputComponent is valid (not NULL)
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("Thrust", this, &APlayerCharacter::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &APlayerCharacter::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRightInput);
}

void APlayerCharacter::ThrustInput(float Val)
{
	float CurrentAcc;
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If speed is below min, increase speed
	if (CurrentForwardSpeed < MinSpeed)
	{
		CurrentAcc = 1.0f * Acceleration;
	}
	else
	{
		// If input is not held down, reduce speed
		CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	}
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	if (CurrentForwardSpeed < MinSpeed)
	{
		CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, 0.0f, MaxSpeed);
	}
	else
	{
		CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
	}
}

void APlayerCharacter::MoveUpInput(float UpValue)
{
	if (UpValue != 0)
	{
		AddMovementInput(GetActorUpVector(), UpValue);
	}
}

void APlayerCharacter::MoveRightInput(float RightValue)
{
	if (RightValue != 0)
	{
		AddMovementInput(GetActorRightVector(), RightValue);
	}
}

void APlayerCharacter::GameTimerExpired()
{
	FName MapName = "FailMap";
	UGameplayStatics::OpenLevel(GetWorld(), MapName);
}

