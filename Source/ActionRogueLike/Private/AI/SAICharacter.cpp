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

AActor* ASAICharacter::GetTargetActor() const
{
	if (auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
	{
		const auto* BlackboardComponent = AIController->GetBlackboardComponent();
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(TEXT("TargetActor")));
	}

	return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (const auto* TargetActor = GetTargetActor(); TargetActor == Pawn)
	{
		return;
	}

	SetTargetActor(Pawn);
	MulticastPawnSeen();
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
		SetTargetActor(InstigatorActor);
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

void ASAICharacter::MulticastPawnSeen_Implementation()
{
	if (IsValid(SpottedWidgetClass))
	{
		if (auto* SpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
			IsValid(SpottedWidget))
		{
			SpottedWidget->SetAttachedActor(this);
			SpottedWidget->AddToViewport(10);
		}
	}
}

bool ASAICharacter::SetTargetActor(AActor* NewTargetActor) const
{
	if (auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
	{
		auto* BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), NewTargetActor);

		return true;
	}

	return false;
}
