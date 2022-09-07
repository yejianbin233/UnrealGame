// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemOptionsWidget.h"

#include "ItemOperatorWidget.h"

void UItemOptionsWidget::Init(FItemOptions Options)
{
	OptionText->SetText(FText::FromString(Options.OptionName));
}
