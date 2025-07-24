#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageType_Stun.generated.h"

UCLASS()
class CRUSTYPIRATE_API UDamageType_Stun : public UDamageType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
	float StunDuration = 0.f;
};
