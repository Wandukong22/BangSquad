// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"
#include "Stage2BossPlayerController.generated.h"

// [전방 선언] 헤더 의존성 최소화 및 컴파일 속도 향상
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UQTEWidget;
class AStage2Boss;

/**
 * ABSPlayerController
 * * [역할]
 * 1. UI (위젯) 관리
 * 2. 네트워크 플레이어 정보 동기화 (닉네임 등)
 * 3. 디버그 및 치트 기능 (서버 검증 포함)
 * 4. 보스전 특수 기믹 (QTE) 입력 처리
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStage2BossPlayerController : public ABSPlayerController
{
	GENERATED_BODY()

public:
	AStage2BossPlayerController();

protected:
	// --- [Core: Lifecycle] ---
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override; // Enhanced Input 바인딩 위치

public:
	// =============================================================
	// [Boss QTE System] - 보스전 연타 기믹
	// =============================================================

	/** * [Client RPC] 보스가 특정 플레이어에게 QTE 시작을 명령
	 * @param BossActor : 결과를 보고할 보스 인스턴스 (서버로 RPC를 보내기 위해 필요)
	 * @param TargetCount : 성공에 필요한 연타 횟수
	 * @param Duration : 제한 시간
	 */
	UFUNCTION(Client, Reliable)
	void Client_StartQTE(AStage2Boss* BossActor, int32 TargetCount, float Duration);

	/** [Client RPC] QTE 종료 처리 (UI 제거 및 입력 모드 복구) */
	UFUNCTION(Client, Reliable)
	void Client_EndQTE(bool bSuccess);

protected:
	/** QTE 입력 액션 바인딩 함수 (A/D 연타) */
	void OnQTETapInput();

	// --- [QTE Configuration] (BP에서 할당) ---

	/** QTE 전용 입력 컨텍스트 (Priority를 높게 설정하여 이동 입력을 덮어써야 함) */
	UPROPERTY(EditDefaultsOnly, Category = "Input|QTE")
	TObjectPtr<UInputMappingContext> QTE_Context;

	/** 연타 액션 (A, D키 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Input|QTE")
	TObjectPtr<UInputAction> QTE_TapAction;

	/** QTE UI 위젯 클래스 (WBP_QTE) */
	UPROPERTY(EditDefaultsOnly, Category = "UI|QTE")
	TSubclassOf<UQTEWidget> QTEWidgetClass;

private:
	// --- [QTE Internal State] ---

	/** 현재 생성된 QTE 위젯 인스턴스 */
	UPROPERTY()
	TObjectPtr<UQTEWidget> QTEWidgetInstance;

	/** QTE를 발동시킨 보스 참조 (Strong Ref로 GC 방지) */
	UPROPERTY()
	TObjectPtr<AStage2Boss> CurrentBossRef;

	int32 CurrentTapCount; // 현재 연타 횟수
	int32 GoalTapCount;    // 목표 연타 횟수
	bool bIsQTEActive;     // 중복 입력 방지 플래그

	
};
