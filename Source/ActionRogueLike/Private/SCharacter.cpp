// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SAttributeComponent.h"
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

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);
}

void ASCharacter::MoveForward(float Value)
{
	auto ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0;
	ControlRotation.Roll = 0;

	AddMovementInput(ControlRotation.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	auto ControlRotation = GetControlRotation();
	// ControlRotation.Yaw = FGenericPlatformMath::Abs(ControlRotation.Yaw);
	ControlRotation.Pitch = 0;
	ControlRotation.Roll = 0;

	const auto RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

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
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(BlackHoleAttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed,
	                                BlackHoleAttackDelay); // :TODO: use animation events
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleAttackProjectileClass);
	/*if (!ensureAlways(BlackHoleAttackProjectileClass))
		return;

	UWorld* World = GetWorld();

	const FVector RightHandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FTransform SpawnTransform = FTransform(GetControlRotation(), RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	World->SpawnActor<AActor>(BlackHoleAttackProjectileClass, SpawnTransform, SpawnParameters);*/
}

void ASCharacter::Dash()
{
	PlayAnimMontage(DashAttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_TimeElapsed, 
									DashAttackDelay); // :TODO: use animation events
}

void ASCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::PrimaryInteract()
{
	if (!InteractionComponent)
		return;

	InteractionComponent->PrimaryInteract();
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ProjectileClassToSpawn)
{
	if (!ensureAlways(ProjectileClassToSpawn))
		return;

	auto* World = GetWorld();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	FCollisionQueryParams AdditionalQueryParams;
	AdditionalQueryParams.AddIgnoredActor(this);

	auto TraceStart = CameraComponent->GetComponentLocation();
	auto TraceEnd = TraceStart + GetControlRotation().Vector() * 5'000;

	FHitResult Hit;
	auto bBlockingHit = World->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity,
	                                                   ObjectQueryParams, Shape, AdditionalQueryParams);
	if (bBlockingHit)
		TraceEnd = Hit.ImpactPoint;

	const auto RightHandSocketLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	auto SpawnRotation = FRotationMatrix::MakeFromX(TraceEnd - RightHandSocketLocation).Rotator();
	const auto SpawnTransform = FTransform(SpawnRotation, RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	World->SpawnActor<AActor>(ProjectileClassToSpawn, SpawnTransform, SpawnParameters);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	float NewHealth,
	float Delta)
{
	if (NewHealth <= 0.f && Delta < 0.f)
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComponent->OnHealthChange.AddDynamic(this, &ASCharacter::OnHealthChanged);
}
