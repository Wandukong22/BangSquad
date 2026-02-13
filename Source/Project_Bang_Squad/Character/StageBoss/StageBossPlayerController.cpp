// Source/Project_Bang_Squad/Character/StageBoss/StageBossPlayerController.cpp

#include "StageBossPlayerController.h"
#include "StageBossGameMode.h" 
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Project_Bang_Squad/UI/Stage/Boss/QTEWidget.h"

AStageBossPlayerController::AStageBossPlayerController()
{
	// ������ ���� (�ʿ�� �߰�)
}

void AStageBossPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// [�ٽ�] QTE�� �Է� ���� ���ؽ�Ʈ �߰� (�켱������ ���� ����)
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (QTE_IMC)
		{
			// Priority�� 1�� �����Ͽ� �⺻ ĳ���� �Է�(���� 0)���� �켱������ ����
			// �̷��� �ϸ� ĳ������ �ٸ� Ű�� ���ĵ� QTE�� ���� ������
			Subsystem->AddMappingContext(QTE_IMC, 1);
		}
	}
}

void AStageBossPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (QTEWidgetInstance && QTEWidgetInstance->IsInViewport())
	{
		QTEWidgetInstance->RemoveFromParent();
		QTEWidgetInstance = nullptr;
	}
}

void AStageBossPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// [����] Enhanced Input �ý����� ����Ͽ� ���ε�
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (QTE_Action)
		{
			EIC->BindAction(QTE_Action, ETriggerEvent::Started, this, &AStageBossPlayerController::Input_QTEInteract);
		}
	}
}

void AStageBossPlayerController::Input_QTEInteract()
{
	// ���� Ŭ���̾�Ʈ���� GŰ ���� -> ������ ����
	// (���⼭ Ŭ���̾�Ʈ �� UI �����̳� ���带 ��� ����ϸ� �������� �� ������)
	Server_SubmitQTEInput();
}

void AStageBossPlayerController::Server_SubmitQTEInput_Implementation()
{
	// [���� ����] GameMode���� ����
	if (UWorld* World = GetWorld())
	{
		if (AStageBossGameMode* GM = Cast<AStageBossGameMode>(World->GetAuthGameMode()))
		{
			// ��Ʈ�ѷ�(this) ������ �Ѱ��־� ���� �������� �ĺ� �����ϰ� ��
			GM->ProcessQTEInput(this);

			// ����� �α� (���� �� ȭ�鿡 ��µ�)
			// UE_LOG(LogTemp, Log, TEXT("Player Controller Sent QTE Input!")); 
		}
	}
}