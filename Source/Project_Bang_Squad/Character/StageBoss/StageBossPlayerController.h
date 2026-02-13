// Source/Project_Bang_Squad/Character/StageBoss/StageBossPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Stage/StagePlayerController.h"
#include "StageBossPlayerController.generated.h"

class UQTEWidget;
class UInputMappingContext;
class UInputAction;

/**
 * ������ ���� ��Ʈ�ѷ�
 * ����: BaseCharacter ���� ����, ���������� QTE �Է��� �޾� ó����.
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageBossPlayerController : public AStagePlayerController
{
	GENERATED_BODY()

public:
	AStageBossPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;

	// [�Է� ����] �����Ϳ��� �Ҵ� �ʿ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> QTE_IMC; // QTE ���� Ű ���� (GŰ)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> QTE_Action;      // QTE �Է� �׼�

	// [����] Ű �Է� �� ȣ��
	void Input_QTEInteract();

	// [����] ������ �Է� ��ȣ ���� (RPC)
	UFUNCTION(Server, Reliable)
	void Server_SubmitQTEInput();

	UPROPERTY()
	TObjectPtr<UQTEWidget> QTEWidgetInstance;
};