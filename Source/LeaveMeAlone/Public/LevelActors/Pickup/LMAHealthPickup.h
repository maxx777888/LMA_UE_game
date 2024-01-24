// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMAHealthPickup.generated.h"

class USphereComponent;
class ALMADefaultCharacter;

UCLASS()
class LEAVEMEALONE_API ALMAHealthPickup : public AActor
{
	GENERATED_BODY()

public:
	ALMAHealthPickup();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* SphereComponent;//Компонент сферы, который будет изображать предмет пикапа здоровья 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float RespawnTime = 5.0f;//Время через сколько появиться новый предмет пикапа здоровья

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "5.0", ClampMax = "100.0"))
	float HealthFromPickup = 100.0f;//Переменная добавляет здоровья на пикапе

	virtual void BeginPlay() override;
	//Метод реализованный в базовом классе Актора, он реагирует на пересечении сетки коллизии с другими Акторами
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	bool GivePickup(ALMADefaultCharacter* Character);//Метод будет проверять если пикапап здоровья взят
	void PickupWasTaken();//Будет запускать таймер респауна и скрывать пикап, после взятия пикапа
	void RespawnPickup();//Будет возвращать видимасть пикапа здоровья
};
