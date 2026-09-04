// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/UI/Menu/MainMenu.h"
#include "Project_Bang_Squad/Data/DataAsset/BSJobData.h"
#include "Project_Bang_Squad/Data/DataAsset/BSMapData.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"

void UBSGameInstance::Init()
{
	Super::Init();
}


void UBSGameInstance::ShowLoadingScreen(UTexture2D* LoadingImage)
{
	// 위젯이 세팅 되어있고, 넘겨받은 이미지가 있을 때만 실행
	if (LoadingWidgetClass && LoadingImage && GetWorld())
	{
		if (UUserWidget* LoadingUI = CreateWidget<UUserWidget>(this, LoadingWidgetClass))
		// 위젯 블루프린트의 이벤트를 호출하여 이미지 전달
		{
			UFunction* Func = LoadingUI->FindFunction(FName("SetLoadingImage"));
			if (Func)
			{
				struct
				{
					UTexture2D* img;
				} Params;
				Params.img = LoadingImage;
				LoadingUI->ProcessEvent(Func, &Params);
			}

			// 화면 꽉 차게 제일 위에 띄움
			LoadingUI->AddToViewport(9999);
		}
	}
}

TSubclassOf<ACharacter> UBSGameInstance::GetCharacterClass(EJobType InJobType) const
{
	if (JobDataAsset)
	{
		return JobDataAsset->GetCharacterClass(InJobType);
	}
	return nullptr;
}

UTexture2D* UBSGameInstance::GetJobIcon(EJobType InJobType) const
{
	if (JobDataAsset)
	{
		return JobDataAsset->GetJobIcon(InJobType);
	}
	return nullptr;
}

FLinearColor UBSGameInstance::GetJobColor(EJobType InJobType) const
{
	if (JobDataAsset)
	{
		return JobDataAsset->GetJobColor(InJobType);
	}
	return FLinearColor::White;
}

void UBSGameInstance::MarkStageAsVisited(EStageIndex Stage, EStageSection Section)
{
	uint32 Key = GetStageKey(Stage, Section);
	VisitedStageKeys.Add(Key);
}

bool UBSGameInstance::HasVisitedStage(EStageIndex Stage, EStageSection Section) const
{
	uint32 Key = GetStageKey(Stage, Section);
	return VisitedStageKeys.Contains(Key);
}

uint32 UBSGameInstance::GetStageKey(EStageIndex Stage, EStageSection Section) const
{
	//비트 연산으로 두 값 섞어서 고유 ID 생성
	return ((uint32)Stage << 16) | (uint32)Section;
}

void UBSGameInstance::ResetAllGameData()
{
	// 1. 캐릭터 및 인벤토리 데이터 초기화
	SavedPlayerCoins.Empty();

	// 2. 스테이지 진행 데이터 초기화
	VisitedStageKeys.Empty();
	StageSaveData.Empty();

	// 3. 체크포인트 및 스테이지 정보 초기화
	CurrentStage = EStageIndex::None;
	SavedCheckpointIndex = 0;

	// 4. 컷신 시청 기록 초기화
	bIsStage1CutscenePlayed = false;
	bIsStage2CutscenePlayed = false;
	bIsStage3CutscenePlayed = false;

	UE_LOG(LogTemp, Warning, TEXT("모든 게임 데이터가 초기화되었습니다."));
}
