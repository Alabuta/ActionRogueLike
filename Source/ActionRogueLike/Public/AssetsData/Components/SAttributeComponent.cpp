// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


namespace SConsoleVariables
{
	TAutoConsoleVariable CVarDamageMultiplier(
		TEXT("su.DamageMultiplier"),
		1.f,
		TEXT("Global damage multiplier for `AttributeComponent`"),
		ECVF_Cheat);
}

USAttributeComponent::USAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Health = HealthMax;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

float USAttributeComponent::GetHealthRatio() const
{
	return Health / HealthMax;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	const auto* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return false;
	}

	if (!OwnerActor->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if (Delta < 0.f)
	{
		Delta *= SConsoleVariables::CVarDamageMultiplier.GetValueOnGameThread();
	}

	Delta = FMath::Clamp(Health + Delta, 0.f, HealthMax) - Health;

	if (OwnerActor->HasAuthority())
	{
		Health += Delta;

		if (Delta != 0.f)
		{
			MulticastHealthChange(InstigatorActor, Health, Delta);
		}

		if (Delta < 0.f && Health == 0.f)
		{
			if (auto* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>(); IsValid(GameMode))
			{
				GameMode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return Delta != 0.f;
}

void USAttributeComponent::Kill(AActor* InstigatorActor)
{
	if (IsAlive())
	{
		ApplyHealthChange(InstigatorActor, -Health);
	}
}

float USAttributeComponent::GetRage() const
{
	return Rage;
}

float USAttributeComponent::GetRageMax() const
{
	return RageMax;
}

float USAttributeComponent::GetRageRatio() const
{
	return Rage / RageMax;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const auto* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return false;
	}

	Delta = FMath::Clamp(Rage + Delta, 0, RageMax) - Rage;

	if (OwnerActor->HasAuthority())
	{
		Rage += Delta;

		if (Delta != 0.f)
		{
			MulticastRageChange(InstigatorActor, Rage, Delta);
		}
	}

	return Delta != 0;
}

void USAttributeComponent::MulticastHealthChange_Implementation(
	AActor* InstigatorActor,
	const float NewValue,
	const float Delta)
{
	OnHealthChange.Broadcast(InstigatorActor, this, NewValue, Delta);
}

void USAttributeComponent::MulticastRageChange_Implementation(
	AActor* InstigatorActor,
	const float NewValue,
	const float Delta)
{
	OnRageChange.Broadcast(InstigatorActor, this, NewValue, Delta);
}

USAttributeComponent* USAttributeComponent::GetAttributeComponent(const AActor* FromActor)
{
	return IsValid(FromActor) ? FromActor->GetComponentByClass<USAttributeComponent>() : nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* Actor)
{
	if (const auto* AttributeComponent = GetAttributeComponent(Actor); IsValid(AttributeComponent))
	{
		return AttributeComponent->IsAlive();
	}

	return false;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);
	DOREPLIFETIME(ThisClass, HealthMax);
	DOREPLIFETIME(ThisClass, Rage);
	DOREPLIFETIME(ThisClass, RageMax);
}
