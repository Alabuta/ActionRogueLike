// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"


void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Invalid instance of BehaviourTree. Assign valid instance in AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}
