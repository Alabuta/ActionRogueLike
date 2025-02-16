// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInteractionComponent.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"
#include "SWorldUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"


 namespace SConsoleVariables
{
	TAutoConsoleVariable CVarDebugDraw(
		TEXT("su.Interaction.DebugDraw"),
		false,
		TEXT("Enable debug draw for `InteractionComponent`"),
		ECVF_Cheat);
}

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USInteractionComponent::PrimaryInteract()
 {
 	if (!IsValid(FocusedActor))
    {
 		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No interactable actor found"));
	    return;
    }

    auto* OwnerPawn = Cast<APawn>(GetOwner());
    ISGameplayInterface::Execute_Interact(FocusedActor, OwnerPawn);
 }

void USInteractionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

 	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable()
 {
 	FVector EyeLocation;
 	FRotator EyeRotation;

    GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);

 	const auto End = EyeLocation + EyeRotation.Vector() * TraceDistance;

 	FCollisionObjectQueryParams ObjectQueryParams;
 	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

 	const auto* World = GetWorld();

 	TArray<FHitResult> Hits;

 	const auto Shape = FCollisionShape::MakeSphere(TraceRadius);

 	const auto bBlockingHit = World->SweepMultiByObjectType(
		 Hits,
		 EyeLocation,
		 End,
		 FQuat::Identity,
		 ObjectQueryParams,
		 Shape);

 	const auto LineColor = bBlockingHit ? FColor::Green : FColor::Red;

#if ENABLE_DRAW_DEBUG
 	if (SConsoleVariables::CVarDebugDraw.GetValueOnGameThread())
 	{
 		DrawDebugLine(World, EyeLocation, End, LineColor, false, 2.f, 0, 2.f);
 	}
#endif

 	FocusedActor = nullptr;

 	for (const FHitResult& Hit : Hits)
 	{
#if ENABLE_DRAW_DEBUG
 		if (SConsoleVariables::CVarDebugDraw.GetValueOnGameThread())
 		{
 			DrawDebugSphere(World, Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 2.f);
 		}
#endif

 		auto* HitActor = Hit.GetActor();
 		if (!IsValid(HitActor))
 		{
 			continue;
 		}

 		if (HitActor->Implements<USGameplayInterface>())
 		{
 			FocusedActor = HitActor;
 			break;
 		}
 	}

 	if (IsValid(FocusedActor))
 	{
 		if (!IsValid(DefaultWidgetInstance) && ensure(DefaultWidgetClass))
 		{
 			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
 		}

 		if (IsValid(DefaultWidgetInstance))
 		{
 			DefaultWidgetInstance->SetAttachedActor(FocusedActor);

 			if (!DefaultWidgetInstance->IsInViewport())
 			{
 				DefaultWidgetInstance->AddToViewport();
 			}
 		}
 	}
    else
    {
    	if (IsValid(DefaultWidgetInstance))
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
    }
}
