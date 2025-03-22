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

	if (!IsValid(ParentSizeBox))
	{
		return;
	}

	FVector2D ScreenPosition;
	const auto bOnScreen = UGameplayStatics::ProjectWorldToScreen(
		GetOwningPlayer(),
		AttachedActor->GetActorLocation() + WorldOffset,
		ScreenPosition);

	if (bOnScreen)
	{
		const auto ViewportDPIScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportDPIScale;

		ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}

	ParentSizeBox->SetVisibility(bOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}
