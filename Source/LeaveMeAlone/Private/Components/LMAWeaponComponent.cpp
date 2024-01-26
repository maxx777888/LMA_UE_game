// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Components/LMAWeaponComponent.h"
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
	if (Weapon && !AnimReloading)
	{
		Weapon->Fire();
		
	}
}

void ULMAWeaponComponent::Reload() 
{

	if (!CanReload()) return;//ѕроверка можем ли мы перезар€дить оружие

	Weapon->ChangeClip();//ќбновл€ем кол-во патронов
	AnimReloading = true;//”станавливаем флаг, что идет перезар€дка
	ACharacter* Character = Cast<ACharacter>(GetOwner());// астуемс€ к главному персонажу
	Character->PlayAnimMontage(ReloadMontage);//«апуск анимации перезар€дки
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();//ƒобавл€ем оружие в начале игры
	InitAnimNotify();
}


// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULMAWeaponComponent::SpawnWeapon() 
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);//—оздаем оружие на сцене
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			/*ћетод присоедин€ет оружие к персонажу. ‘ункци€ имеет следующие аргументы: 
				  какому компоненту мы присоедин€ем оружие.
				—труктура правил присоединени€.
				Ќазвание сокета,к которому примен€етс€ присоединение.*/
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}
}

void ULMAWeaponComponent::InitAnimNotify() 
{
	if (!ReloadMontage)//ѕроверка установлен ли пользователем сам анимационный монтаж
		return;

	//—оздаем переменную, в которую записываем массив всех уведомлений, доступных дл€ добавлени€ к анимации
	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)//перебираем массив в поисках нужного уведомлени€
	{
		//”ведомление которое будем искать
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			//подписываемс€ через делегат на данное уведомление
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
	return !AnimReloading;
}

