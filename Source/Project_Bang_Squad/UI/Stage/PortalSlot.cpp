// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSlot.h"

#include "Components/Image.h"

void UPortalSlot::SetActive(bool bIsActive)
{
	if (Img_User_Icon)
	{
		float Opacity = bIsActive ? 1.f : 0.2f;
		Img_User_Icon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, Opacity));
	}
}
