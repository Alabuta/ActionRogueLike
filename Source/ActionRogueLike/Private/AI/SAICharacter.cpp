// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "SWorldUserWidget.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SAttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

bool ASAICharacter::SetNewTarget(AActor* NewTargetActor) const
{
	if (auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
	{
		auto* BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), NewTargetActor);

		return true;
	}

	return false;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (SetNewTarget(Pawn))
	{
		DrawDebugString(GetWorld(),	GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::Orange, 2.f, true);
	}
}

void ASAICharacter::OnHealthChanged(
	AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	const float NewHealth,
	const float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	if (IsValid(InstigatorActor))
	{
		SetNewTarget(InstigatorActor);
	}

	if (!IsValid(ActiveHealthBarWidget) && IsValid(HealthBarWidgetClass))
	{
		ActiveHealthBarWidget = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (IsValid(ActiveHealthBarWidget))
		{
			ActiveHealthBarWidget->AttachedActor = this;
			ActiveHealthBarWidget->AddToViewport();
		}
	}

	auto* SkeletalMeshComponent = GetMesh();
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}

	SkeletalMeshComponent->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

	if (NewHealth <= 0.f)
	{
		if (const auto* AIController = Cast<ASAIController>(GetController()); IsValid(AIController))
		{
			AIController->GetBrainComponent()->StopLogic(TEXT("Killed"));
		}

		SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
		SkeletalMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));

		SetLifeSpan(10.f);
	}
}
