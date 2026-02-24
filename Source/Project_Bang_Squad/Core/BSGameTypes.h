// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSGameTypes.generated.h"

// ���� ����
class UInputMappingContext;
class UInputAction;

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

UENUM(BlueprintType)
enum class EMiniGamePhase : uint8
{
	Waiting,
	Playing,
	Finished
};

// =============================================================
// [2] �ű� ������ (QTE �ý��� �߰�)
// =============================================================

/** QTE ���� */
UENUM(BlueprintType)
enum class EQTEType : uint8
{
	None,
	Stage1_Wipe,     // 1�������� ����� ��� (GŰ)
	Stage2_Mash,     // 2�������� �Ź��� Ż�� (A/D ��Ÿ)
	Stage3_Sequence, // 3�������� (����)
};

/** QTE ������ (���� + �Է�) */
USTRUCT(BlueprintType)
struct FQTEConfig
{
	GENERATED_BODY()

	// ����� ���� (��: WBP_QTE_Mash)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	// ����� �Է� ���ؽ�Ʈ (��: IMC_QTE_AD)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> InputContext;

	// ������ �Է� �׼� (��: IA_Tap)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;
};