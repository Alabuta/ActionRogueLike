// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


void USWorldUserWidget::SetAttachedActor(const AActor* NewAttachedActor)
{
	AttachedActor = NewAttachedActor;
}

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		UE_LOGFMT(LogTemp, Warning, "Attached actor is no longer valid, removing health widget.");
		RemoveFromParent();
		return;
	}

	if (!ParentSizeBox)
	{
		return;
	}

	if (FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
	{
		const auto ViewportDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportDPIScale;

		ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
}
