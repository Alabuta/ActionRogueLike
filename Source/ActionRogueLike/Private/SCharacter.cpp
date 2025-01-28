// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "TimerManager.h"
#include "Components/SAttributeComponent.h"
#include "Components/SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


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

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->OnHealthChange.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("PrimaryAttack"), IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction(TEXT("BlackHoleAttack"), IE_Pressed, this, &ASCharacter::BlackHoleAttack);

	PlayerInputComponent->BindAction(TEXT("PrimaryInteract"), IE_Pressed, this, &ASCharacter::PrimaryInteract);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ASCharacter::Dash);
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
	GetWorldTimerManager()
		.SetTimer(
			TimerHandle_PrimaryAttack,
			this,
			&ASCharacter::PrimaryAttack_TimeElapsed,
			PrimaryAttackDelay); // :TODO: use animation events
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(BlackHoleAttackAnim);
	GetWorldTimerManager()
		.SetTimer(
			TimerHandle_BlackHoleAttack,
			this,
			&ASCharacter::BlackHoleAttack_TimeElapsed,
			BlackHoleAttackDelay); // :TODO: use animation events
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleAttackProjectileClass);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(DashAttackAnim);
	GetWorldTimerManager()
		.SetTimer(TimerHandle_Dash,
		          this,
		          &ASCharacter::Dash_TimeElapsed,
		          DashAttackDelay); // :TODO: use animation events
}

void ASCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::StartAttackFXs(UAnimMontage* AttackAnimMontage)
{
	PlayAnimMontage(AttackAnimMontage);

	UGameplayStatics::SpawnEmitterAttached(
		CastingVfx,
		GetMesh(),
		RightHandSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent != nullptr)
	{
		InteractionComponent->PrimaryInteract();
	}
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

	const auto RightHandSocketLocation = GetMesh()->GetSocketLocation(RightHandSocketName);

	FHitResult Hit;
	auto bBlockingHit = World->SweepSingleByObjectType(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectQueryParams,
		Shape,
		AdditionalQueryParams);

	auto SpawnRotation = bBlockingHit
		                     ? FRotationMatrix::MakeFromX(Hit.ImpactPoint - RightHandSocketLocation).Rotator()
		                     : FRotationMatrix::MakeFromX(TraceEnd - RightHandSocketLocation).Rotator();

	const auto SpawnTransform = FTransform(SpawnRotation, RightHandSocketLocation);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	World->SpawnActor<AActor>(ProjectileClassToSpawn, SpawnTransform, SpawnParameters);
}

void ASCharacter::OnHealthChanged(
	AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	const float NewHealth,
	const float Delta)
{
	if (Delta < 0.f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		if (NewHealth <= 0.f)
		{
			DisableInput(Cast<APlayerController>(GetController()));
		}
	}
}
