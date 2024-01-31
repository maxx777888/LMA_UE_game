// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Components/LMAWeaponComponent.h"
#include "Player/LMADefaultCharacter.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/LMABaseWeapon.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void ULMAWeaponComponent::Fire() 
{
	auto Player = Cast<ALMADefaultCharacter>(GetOwner());
	if (IsValid(Player))
	{
		if (Weapon && !AnimReloading && Player->IsSprint==false)
		{
			Weapon->Fire();
		}
	}
	
}

void ULMAWeaponComponent::StopFire() 
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}



void ULMAWeaponComponent::ReloadNeeded() 
{
	if (!CanReload()) return;// Проверка можем ли мы перезарядить оружие
	ACharacter* Character = Cast<ACharacter>(GetOwner()); // Кастуемся к главному персонажу
	if (Character->GetVelocity().Length() >= 300.f) return; // Если персонаж бежит спринт, то отмена перезарядки
	Character->PlayAnimMontage(ReloadMontage); // Запуск анимации перезарядки
	Weapon->StopFire();//Остановка и запрет стрельбы
	Weapon->ChangeClip();// Обновляем кол-во патронов
	AnimReloading = true;// Устанавливаем флаг, что идет перезарядка
}

bool ULMAWeaponComponent::GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const
{
	if (Weapon)
	{
		AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
		return true;
	}

	return false;
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();//Добавляем оружие в начале игры
	Weapon->noBullets.AddUObject(this, &ULMAWeaponComponent::ReloadNeeded);

	InitAnimNotify();
}


// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULMAWeaponComponent::SpawnWeapon() //Добавляем оружие персонажу на сцене
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);//Создаем оружие на сцене
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			/*Метод присоединяет оружие к персонажу. Функция имеет следующие аргументы: 
				К какому компоненту мы присоединяем оружие.
				Структура правил присоединения.
				Название сокета,к которому применяется присоединение.*/
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}
}

void ULMAWeaponComponent::InitAnimNotify() 
{
	if (!ReloadMontage)//Проверка установлен ли пользователем сам анимационный монтаж
		return;

	//Создаем переменную, в которую записываем массив всех уведомлений, доступных для добавления к анимации
	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)//перебираем массив в поисках нужного уведомления
	{
		//Уведомление которое будем искать
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			//подписываемся через делегат на данное уведомление
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh) 
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	if (!Weapon->isClipFull()) 
	{
		return !AnimReloading;
	}
	else
	{
		return false;
	}
}

