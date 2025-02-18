// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "SWorldUserWidget.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SActionComponent.h"
#include "Components/SAttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComponent"));
	ActionComponent = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	if (IsValid(GetCapsuleComponent()))
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	}

	if (auto* SkeletalMeshComponent = GetMesh(); IsValid(SkeletalMeshComponent))
	{
		SkeletalMeshComponent->SetGenerateOverlapEvents(true);
	}
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(PawnSensingComponent))
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	}

	if (IsValid(AttributeComponent))
	{
		AttributeComponent->OnHealthChange.AddDynamic(this, &ASAICharacter::OnHealthChanged);
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (SetActorTarget(Pawn))
	{
		DrawDebugString(GetWorld(),	GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::Orange, 2.f, true);
	}
}

void ASAICharacter::OnHealthChanged(
	AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	const float NewValue,
	const float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	if (InstigatorActor != this && IsValid(InstigatorActor))
	{
		SetActorTarget(InstigatorActor);
	}

	if (!IsValid(ActiveHealthBarWidget) && IsValid(HealthBarWidgetClass))
	{
		ActiveHealthBarWidget = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (IsValid(ActiveHealthBarWidget))
		{
			ActiveHealthBarWidget->SetAttachedActor(this);
			ActiveHealthBarWidget->AddToViewport();
		}
	}

	auto* SkeletalMeshComponent = GetMesh();

	if (IsValid(SkeletalMeshComponent))
	{
		SkeletalMeshComponent->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
	}

	if (NewValue <= 0.f)
	{
		if (const auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
		{
			AIController->GetBrainComponent()->StopLogic(TEXT("Killed"));
		}

		if (IsValid(SkeletalMeshComponent))
		{
			SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
			SkeletalMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		}

		if (IsValid(GetCapsuleComponent()))
		{
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (IsValid(GetCharacterMovement()))
		{
			GetCharacterMovement()->DisableMovement();
		}

		SetLifeSpan(10.f);
	}
}

bool ASAICharacter::SetActorTarget(AActor* NewTargetActor) const
{
	if (auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
	{
		auto* BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), NewTargetActor);

		return true;
	}

	return false;
}
