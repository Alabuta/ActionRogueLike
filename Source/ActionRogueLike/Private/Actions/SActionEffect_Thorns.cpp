// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect_Thorns.h"

#include "SGameplayFunctionLibrary.h"
#include "Components/SActionComponent.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/Actor.h"


USActionEffect_Thorns::USActionEffect_Thorns()
{
	Duration = 0.f;
	Period = 0.f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	const auto* ActionComponent = GetOwningComponent();
	if (!ensure(IsValid(ActionComponent)))
	{
		return;
	}

	auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(ActionComponent->GetOwner());
	if (!ensure(IsValid(AttributeComponent)))
	{
		return;
	}

	AttributeComponent->OnHealthChange.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	const auto* ActionComponent = GetOwningComponent();
	if (!ensure(IsValid(ActionComponent)))
	{
		return;
	}

	auto* AttributeComponent = USAttributeComponent::GetAttributeComponent(ActionComponent->GetOwner());
	if (!ensure(IsValid(AttributeComponent)))
	{
		return;
	}

	AttributeComponent->OnHealthChange.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);

	Super::StopAction_Implementation(Instigator);
}

void USActionEffect_Thorns::OnHealthChanged(
	AActor* InstigatorActor,
	USAttributeComponent* OwningComponent,
	const float NewValue,
	const float Delta)
{
	if (Delta >= 0.f)
	{
		return;
	}

	if (!IsValid(InstigatorActor))
	{
		return;
	}

	const auto* ActionComponent = GetOwningComponent();
	if (!ensure(IsValid(ActionComponent)))
	{
		return;
	}

	auto* OwnerActor = ActionComponent->GetOwner();
	if (!ensure(IsValid(OwnerActor)))
	{
		return;
	}

	if (InstigatorActor == OwnerActor)
	{
		return;
	}

	const int32 ReflectedDamage = FMath::RoundToInt(Delta * ReflectionFraction);
	if (ReflectedDamage >= 0)
	{
		return;
	}

	USGameplayFunctionLibrary::ApplyDamage(OwnerActor, InstigatorActor, -ReflectedDamage);
}
