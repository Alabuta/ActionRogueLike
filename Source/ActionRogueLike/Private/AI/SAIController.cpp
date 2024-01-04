// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	auto* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	auto* BlackboardComponent = GetBlackboardComponent();
	if (PlayerPawn != nullptr && BlackboardComponent != nullptr)
	{
		BlackboardComponent->SetValueAsVector(TEXT("MoveToLocation"), PlayerPawn->GetActorLocation());
		BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
	}
}
