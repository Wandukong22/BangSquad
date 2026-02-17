#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project_Bang_Squad/Game/Interface/SaveInterface.h"
#include "GemStatue.generated.h"

class ACenterStatueManager;

// 보석 액터를 전방 선언 (실제 구현은 그냥 AActor 상속받고 피격 시 Destroy 호출하면 됨)
class AActor;

UCLASS()
class PROJECT_BANG_SQUAD_API AGemStatue : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	AGemStatue();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "BS|Save")
	FName PuzzleID;

	//인터페이스 함수
	virtual FName GetSaveID() const override { return PuzzleID; }
	virtual void SaveActorData(FActorSaveData& OutData) override;
	virtual void LoadActorData(const FActorSaveData& InData) override;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// =================================================================
	// 컴포넌트
	// =================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StatueMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* HiddenAddonMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link")
	ACenterStatueManager* CenterStatue;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link")
	TArray<AActor*> TargetGems;

	UFUNCTION()
	void CheckGemStatus(AActor* DestroyedGem);

private:
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated = false;

	int32 CurrentGemCount = 0;

	UFUNCTION()
	void OnRep_IsActivated();
};