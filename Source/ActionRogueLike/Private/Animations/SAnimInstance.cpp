// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/SAnimInstance.h"

#include "GameplayTagContainer.h"
#include "Components/SActionComponent.h"
#include "GameFramework/Actor.h"


void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const auto* OwningActor = GetOwningActor(); IsValid(OwningActor))
	{
		ActionComponent = OwningActor->GetComponentByClass<USActionComponent>();
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag{FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned"))};

	if (IsValid(ActionComponent))
	{
		bStunned = ActionComponent->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
