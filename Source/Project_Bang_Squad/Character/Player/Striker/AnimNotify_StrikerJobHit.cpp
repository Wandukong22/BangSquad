#include "Project_Bang_Squad/Character/Player/Striker/AnimNotify_StrikerJobHit.h" 
#include "Project_Bang_Squad/Character/StrikerCharacter.h" 

void UAnimNotify_StrikerJobHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
    Super::Notify(MeshComp, Animation, EventReference);

    if (MeshComp && MeshComp->GetOwner())
    {
        AStrikerCharacter* Striker = Cast<AStrikerCharacter>(MeshComp->GetOwner());
        if (Striker)
        {
            Striker->ApplyJobAbilityHit();
        }
    }
}