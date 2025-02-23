// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetAttachedActor(const AActor* NewAttachedActor);

protected:

	UPROPERTY(EditDefaultsOnly, Category="UI")
	FVector WorldOffset;

	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(ExposeOnSpawn=true))
	TObjectPtr<const AActor> AttachedActor{nullptr};

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox{nullptr};

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
