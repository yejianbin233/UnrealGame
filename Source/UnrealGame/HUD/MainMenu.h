// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"


class UTextBlock;

/**
 * 
 */
UCLASS()
class UNREALGAME_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* HostGame;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* OnlineGame;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SaveGame;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Option;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Quit;

protected:

private:
	
};
