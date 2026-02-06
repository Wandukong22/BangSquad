// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSGameTypes.generated.h"

UENUM(BlueprintType)
enum class EJobType : uint8
{
	None,
	Titan,
	Striker,
	Mage,
	Paladin
};

UENUM(BlueprintType)
enum class EStageIndex : uint8
{
	None,
	Lobby,
	Stage1,
	Stage2,
	Stage3
};

UENUM(BlueprintType)
enum class EStageSection : uint8
{
	Unknown,
	Menu,
	Main,
	MiniGame,
	Boss
};