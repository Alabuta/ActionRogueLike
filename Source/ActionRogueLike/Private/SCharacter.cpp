// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SInteractionComponent.h"
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
	PlayerInputComponent->BindAction("BlackHoleAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
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

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams AdditionalQueryParams;
	AdditionalQueryParams.AddIgnoredActor(this);

	FVector TraceStart = CameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + GetControlRotation().Vector() * 10'000;

	FHitResult Hit;
	bool bBlockingHit = World->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity,
	                                                   ObjectQueryParams, Shape, AdditionalQueryParams);
	if (bBlockingHit)
		TraceEnd = Hit.ImpactPoint;

	const FVector RightHandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FRotator SpawnRotation = FRotationMatrix::MakeFromX(TraceEnd - RightHandSocketLocation).Rotator();
	const FTransform SpawnTransform = FTransform(SpawnRotation, RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	World->SpawnActor<AActor>(ProjectileClass, SpawnTransform, SpawnParameters);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed,
									PrimaryAttackDelay); // :TODO: use animation events
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	UWorld* World = GetWorld();

	const FVector RightHandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FTransform SpawnTransform = FTransform(GetControlRotation(), RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	World->SpawnActor<AActor>(BlackHoleAttackProjectileClass, SpawnTransform, SpawnParameters);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent == nullptr)
		return;

	InteractionComponent->PrimaryInteract();
}
