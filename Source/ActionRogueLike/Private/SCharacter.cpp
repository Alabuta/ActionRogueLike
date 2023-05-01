// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SInteractionComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0;
	ControlRotation.Roll = 0;

	AddMovementInput(ControlRotation.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRotation = GetControlRotation();
	// ControlRotation.Yaw = FGenericPlatformMath::Abs(ControlRotation.Yaw);
	ControlRotation.Pitch = 0;
	ControlRotation.Roll = 0;

	const FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed,
	                                PrimaryAttackDelay); // :TODO: use animation events
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	const FVector RightHandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FTransform SpawnTransform = FTransform(GetControlRotation(), RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParameters);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent == nullptr)
		return;

	InteractionComponent->PrimaryInteract();
}
