// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Components/SActionComponent.h"
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
	ActionComponent = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComponent"));

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

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ASCharacter::SprintStop);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComponent->GetComponentLocation();
}

void ASCharacter::HealSelf(const float Amount)
{
	if (IsValid(AttributeComponent))
	{
		AttributeComponent->ApplyHealthChange(this, Amount);
	}
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

void ASCharacter::SprintStart()
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->StartActionByName(this, TEXT("Sprint"));
	}
}

void ASCharacter::SprintStop()
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->StopActionByName(this, TEXT("Sprint"));
	}
}

void ASCharacter::PrimaryAttack()
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->StartActionByName(this, TEXT("PrimaryAttack"));
	}
}

void ASCharacter::BlackHoleAttack()
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->StartActionByName(this, TEXT("BlackHoleAttack"));
	}
}

void ASCharacter::Dash()
{
	if (IsValid(ActionComponent))
	{
		ActionComponent->StartActionByName(this, TEXT("Dash"));
	}
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryInteract();
	}
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
		else
		{
			if (IsValid(OwningComponent))
			{
				OwningComponent->ApplyRageChange(InstigatorActor, -Delta);
			}
		}
	}
}
