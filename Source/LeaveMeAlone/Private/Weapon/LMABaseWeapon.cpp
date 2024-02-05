// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Weapon/LMABaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"

//DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);//лог будет отображать количество боеприпасов

// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);

}

void ALMABaseWeapon::Fire() 
{
	if (IsCurrentClipEmpty())
	{
		return;
	}
	else
	{
		// Запуск таймера для стрельбы
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ALMABaseWeapon::Shoot, FireTimer, true);
	}
		
}

void ALMABaseWeapon::StopFire()
{
	if (GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
	}	
}

void ALMABaseWeapon::ChangeClip() 
{
	if (!isClipFull())
	{
		CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
	}
}

bool ALMABaseWeapon::isClipFull()
{
	if (CurrentAmmoWeapon.Bullets == AmmoWeapon.Bullets)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;//Заполняем патронами магазин в начале игры
}

void ALMABaseWeapon::Shoot() 
{
	const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");//Получили расположение сокета дула
	const FVector TraceStart = SocketTransform.GetLocation();//Задали изначальную точку трассировки
	//Получили направление стрельбы, в данном случае это координата X нашего сокета
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;//Получили точку конца трассировки
	// Нарисовали линию трасировки, параметры метода 1.Указатель на мир 2.Точка старта прорисовки линии 3. Конечная точка 4. Цвет линии
	//5. Будет линия прорисована постоянно или на короткое время 6. Время которое линию будет видно 7. Глубина 8. Толщина линии
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);

	FHitResult HitResult;
	//LineTraceSingleByChannel - вернет информацию о пересечении линии трассировки с самым первым объектом
	//Параметры метода: 1. Информация о точке пересечения ( Структура FHitResult) 2. Начальная точка трейса 3. Конечная точка трейса
	//4. Канал коллизии с которым работает наш канал трассировки, все остальные объекты будут игнорироваться
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	if (HitResult.bBlockingHit)//Проверка на попадание
	{
		//Если попали, то прорисовываем сферическое пространство с помощью метода DrawDebugSphere
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
	}
	DecrementBullets();
}

void ALMABaseWeapon::DecrementBullets() 
{
	CurrentAmmoWeapon.Bullets--;
	
	//UE_LOG(LogWeapon, Display, TEXT("Bullets = %s"), *FString::FromInt(CurrentAmmoWeapon.Bullets));

	if (IsCurrentClipEmpty())//Если обойма магазина патронов пустая, останавливаем таймер стрельбы
	{
		StopFire();
		noBullets.Broadcast();
	}
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

// Called every frame
void ALMABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

