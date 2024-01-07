// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	auto* AIController = Cast<ASAIController>(GetController());
	if (AIController != nullptr)
	{
		auto* BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), Pawn);

		DrawDebugString(GetWorld(),	GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::Orange, 2.f, true);
	}
}
