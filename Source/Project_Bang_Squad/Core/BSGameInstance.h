// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSGameTypes.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Project_Bang_Squad/Game/Interface/SaveInterface.h"
#include "BSGameInstance.generated.h"

class UBSJobData;

UCLASS()
class PROJECT_BANG_SQUAD_API UBSGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	virtual void Init() override;

private:
	//최종 선택된 직업
	UPROPERTY()
	EJobType PlayerJob = EJobType::None;

	//닉네임 저장
	UPROPERTY()
	FString UserNickname;

public:
	FORCEINLINE EJobType GetPlayerJob() const { return PlayerJob; }
	void SetPlayerJob(EJobType NewJob) { PlayerJob = NewJob; }

	void SetUserNickname(FString NewName) { UserNickname = NewName; };
	FString GetUserNickname() const { return UserNickname; }

#pragma region JobData
	UPROPERTY(EditDefaultsOnly, Category = "BS|Data")
	TObjectPtr<UBSJobData> JobDataAsset;
	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	TSubclassOf<ACharacter> GetCharacterClass(EJobType InJobType) const;
	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	UTexture2D* GetJobIcon(EJobType InJobType) const;
	UFUNCTION(BlueprintCallable, Category = "BS|Data")
	FLinearColor GetJobColor(EJobType InJobType) const;
#pragma endregion

#pragma region Portal

public:
	UPROPERTY()
	TSet<uint32> VisitedStageKeys;

	//방문한 거 표시
	UFUNCTION()
	void MarkStageAsVisited(EStageIndex Stage, EStageSection Section);

	//방문 했는지 확인
	UFUNCTION()
	bool HasVisitedStage(EStageIndex Stage, EStageSection Section) const;

	UFUNCTION()
	FORCEINLINE EStageIndex GetCurrentStage() const { return CurrentStage; }

	UFUNCTION()
	void SetCurrentStage(EStageIndex InStage) { CurrentStage = InStage; }

private:
	uint32 GetStageKey(EStageIndex Stage, EStageSection Section) const;

	UPROPERTY()
	EStageIndex CurrentStage = EStageIndex::None;
#pragma endregion

#pragma region Checkpoint

public:
	int32 GetSavedCheckpointIndex() const { return SavedCheckpointIndex; }
	void SetSavedCheckpointIndex(int32 NewIndex) { SavedCheckpointIndex = NewIndex; }
	void InitSavedCheckpointIndex() { SavedCheckpointIndex = 0; }

private:
	UPROPERTY()
	int32 SavedCheckpointIndex = 0;
#pragma endregion

#pragma region Cutscene Data

public:
	// 스테이지 1 컷신 시청 여부
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BS|CutScene")
	bool bIsStage1CutscenePlayed = false;

	// 스테이지 2 컷신 시청 여부
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BS|CutScene")
	bool bIsStage2CutscenePlayed = false;

	// 스테이지 3 컷신 시청 여부
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BS|CutScene")
	bool bIsStage3CutscenePlayed = false;
#pragma endregion
	// =========================================================================
	// 코인 시스템 (Coin System Persistence)
	// =========================================================================
public:
	// Key 타입을 FString으로 변경
	UPROPERTY()
	TMap<FString, int32> SavedPlayerCoins;

	// 코인 저장 (레벨 이동 전 호출)
	// [수정] 인자 타입 변경
	void SaveCoinToInstance(FString PlayerKey, int32 Amount)
	{
		SavedPlayerCoins.FindOrAdd(PlayerKey) = Amount;

		// 로그로 확인
		UE_LOG(LogTemp, Warning, TEXT("💾 [GameInstance] Saved for %s: %d G"), *PlayerKey, Amount);
	}

	//  코인 불러오기
	int32 LoadCoinFromInstance(FString PlayerKey)
	{
		if (SavedPlayerCoins.Contains(PlayerKey))
		{
			int32 Val = SavedPlayerCoins[PlayerKey];
			UE_LOG(LogTemp, Warning, TEXT("📂 [GameInstance] Loaded for %s: %d G"), *PlayerKey, Val);
			return Val;
		}
		UE_LOG(LogTemp, Warning, TEXT("📂 [GameInstance] No data for %s (Start 0 G)"), *PlayerKey);
		return 0;
		//return 1000;
	}

	// =========================================================================
	//  로딩 UI 시스템
	// =========================================================================
	UPROPERTY(EditAnywhere, Category = "UI|Loading")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	// 스테이지 번호가 아닌, 맵 데이터에 있는 이미지 자체를 받는다.
	UFUNCTION(BlueprintCallable, Category = "UI|Loading")
	void ShowLoadingScreen(UTexture2D* LoadingImage);

	// 엔딩 후 모든 데이터를 초기화하는 함수
	UFUNCTION(BlueprintCallable, Category = "BS|System")
	void ResetAllGameData();

#pragma region Save Actor

public:
	UPROPERTY()
	TMap<FName, FActorSaveData> StageSaveData;

	void SaveDataToInstance(FName ID, const FActorSaveData& Data) { StageSaveData.Add(ID, Data); }
	FActorSaveData* GetDataFromInstance(FName ID) { return StageSaveData.Find(ID); }

#pragma endregion
};
