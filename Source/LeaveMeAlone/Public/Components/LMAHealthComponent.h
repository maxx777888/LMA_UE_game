// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAHealthComponent.generated.h"

//Делегат который будет сообщать всем что персонаж умер
DECLARE_MULTICAST_DELEGATE(FOnDeath) 
//Делегат для получения информации об изменении здоровья
//Бродкастить данный делегат необходимо в двух местах, при старте игры и при получении урона
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	ULMAHealthComponent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; } //Возвращает текущее состояние здоровья

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;//Проверяет если у персонажа осталось здоровье
	//Будет прибавлять здоровье и возвращать значение правды, если здоровье было успешно прибавлено
	bool AddHealth(float NewHealth);
	//Будет проверять если компонент здоровья полный
	bool IsHealthFull() const;

	FOnDeath OnDeath;//Задекларированный делегат отслеживающий смерть персонажа
	FOnHealthChanged OnHealthChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	
private:
	float Health = 0.0f;
	/*
	В сигнатуре делегата указано, что он является DYNAMIC_MULTICAST,
	а значит мы обязаны созданную функцию пометить макросом
	UFUNCTION.Аргументы функции должны полностью совпадать с аргументами делегата.
	*/
	UFUNCTION()
	//Функция вызывается при получение урона
	//FTakeAnyDamageSignature, AActor, OnTakeAnyDamage – это параметры самого делегата
	void OnTakeAnyDamage(
		AActor* DamagedActor, float Damage, const class UDamageType* DamageType, 
			class AController* InstigatedBy, AActor* DamageCauser);
	/*Оставшиеся параметры :
		● Актор, которому был нанесен ущерб.
		● Количественная характеристика ущерба.
		● Тип ущерба.
		● Контроллер, который ответственный за ущерб.
		● Актор, ответственный за ущерб.*/
};
