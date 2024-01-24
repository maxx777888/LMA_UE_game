// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LMAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LEAVEMEALONE_API ALMAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALMAPlayerController();

protected:
	virtual void BeginPlay() override;
	//ѕереопределение функции дл€ правильной работы камеры после смерти персонажа
	//более подробное объ€снение на странице презентации 6.83-6.84
	virtual void BeginSpectatingState() override;

	
};
