// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"


ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
