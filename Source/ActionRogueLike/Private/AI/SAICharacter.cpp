// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "BrainComponent.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComponent"));
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComponent->OnHealthChange.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

bool ASAICharacter::SetNewTarget(AActor* NewTargetActor) const
{
	if (auto* AIController = Cast<ASAIController>(GetController()); AIController != nullptr)
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
	float NewHealth,
	float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	if (InstigatorActor != this)
	{
		SetNewTarget(InstigatorActor);
	}

	GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

	if (NewHealth <= 0.f)
	{
		if (const auto* AIController = Cast<ASAIController>(GetController()); AIController != nullptr)
		{
			AIController->GetBrainComponent()->StopLogic(TEXT("Killed"));
		}

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

		SetLifeSpan(10.f);
	}
}
