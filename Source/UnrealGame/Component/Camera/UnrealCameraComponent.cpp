// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealCameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealGame/Character/BlasterCharacter.h"
#include "UnrealGame/ConsoleVariable/ConsoleVariableActor.h"
#include "UnrealGame/DataAsset/UnrealGameAssetManager.h"
#include "UnrealGame/Enums/UnrealGameEnumInterface.h"
#include "UnrealGame/HUD/Camera/CameraWidget.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY(CameraLog);

// Sets default values for this component's properties
UUnrealCameraComponent::UUnrealCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(this);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	CameraViewPointModes.Add(ECameraViewPointMode::FirstPerson);
	CameraViewPointModes.Add(ECameraViewPointMode::ThreePerson);
	CameraViewPointModes.Add(ECameraViewPointMode::Debug_RightPoint);
	CameraViewPointModes.Add(ECameraViewPointMode::Debug_TopPoint);
	// ...
}

void UUnrealCameraComponent::BindCameraInput()
{
	if (!bIsBindInput)
	{
		if (PlayerController && PlayerController->InputComponent)
		{
			if (UEnhancedInputComponent* EInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				if (EIA_Shot)
				{
					EInputComponent->BindAction(EIA_Shot, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::Shot);
				}
				if (EIA_Zoom)
				{
					EInputComponent->BindAction(EIA_Zoom, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::ZoomFOV);
				}
				if (EIA_CloseCamera)
				{
					EInputComponent->BindAction(EIA_CloseCamera, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::CloseCamera);
				}

				if (EIA_EnterDebugMode)
				{
					EInputComponent->BindAction(EIA_EnterDebugMode, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::EnterDebugModePressed);
				}

				if (EIA_LeaveDebugMode)
				{
					EInputComponent->BindAction(EIA_LeaveDebugMode, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::LeaveDebugModeRelease);
				}

				if (EIA_AdjustArmLength)
				{
					EInputComponent->BindAction(EIA_AdjustArmLength, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::AdjustArmLength);
				}

				if (EIA_SelectViewPoint1)
				{
					EInputComponent->BindAction(EIA_SelectViewPoint1, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::SelectViewPoint1Pressed);
				}

				if (EIA_SelectViewPoint2)
				{
					EInputComponent->BindAction(EIA_SelectViewPoint2, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::SelectViewPoint2Pressed);
				}

				if (EIA_SelectViewPoint3)
				{
					EInputComponent->BindAction(EIA_SelectViewPoint3, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::SelectViewPoint3Pressed);
				}

				if (EIA_SelectViewPoint4)
				{
					EInputComponent->BindAction(EIA_SelectViewPoint4, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::SelectViewPoint4Pressed);
				}

				if (EIA_Reset)
				{
					EInputComponent->BindAction(EIA_Reset, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::ResetPressed);
				}

				if (EIA_TargetOffsetX)
				{
					EInputComponent->BindAction(EIA_TargetOffsetX, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::AdjustXOffset);
				}

				if (EIA_TargetOffsetY)
				{
					EInputComponent->BindAction(EIA_TargetOffsetY, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::AdjustYOffset);
				}

				if (EIA_TargetOffsetZ)
				{
					EInputComponent->BindAction(EIA_TargetOffsetZ, ETriggerEvent::Triggered, this, &UUnrealCameraComponent::AdjustZOffset);
				}
			}
		}
		bIsBindInput = true;
	}
}


void UUnrealCameraComponent::SetCameraViewPoint(ECameraViewPointMode InViewPointMode)
{
	ECameraViewPointMode PreCameraViewPointMode = ViewPointMode;
	ViewPointMode = InViewPointMode;
	
	switch (ViewPointMode)
	{
		case ECameraViewPointMode::FirstPerson:
			{
				if (FirstToThreeInterpolationTimeline.IsPlaying())
				{
					FirstToThreeInterpolationTimeline.Stop();
				}
				SwitchFirstPersonViewpoint();
				break;
			}
		case ECameraViewPointMode::ThreePerson:
			{
				bool bIsFirstToThree = PreCameraViewPointMode == ECameraViewPointMode::FirstPerson && ViewPointMode == ECameraViewPointMode::ThreePerson;
				if (bIsFirstToThree)
				{
					if (!FirstToThreeInterpolationTimeline.IsPlaying())
					{
						FirstToThreeInterpolationTimeline.PlayFromStart();
					}
				}
				else
				{
					SwitchThreePersonViewpoint();
				}
				break;
			};

		case ECameraViewPointMode::Debug_RightPoint:
			{
				if (FirstToThreeInterpolationTimeline.IsPlaying())
				{
					FirstToThreeInterpolationTimeline.Stop();
				}
				SwitchDebugRightViewpoint();
				break;
			}
		
		case ECameraViewPointMode::Debug_TopPoint:
			{
				if (FirstToThreeInterpolationTimeline.IsPlaying())
				{
					FirstToThreeInterpolationTimeline.Stop();
				}
				SwitchDebugTopViewPoint();
				break;
			}
		default:
			{
				if (FirstToThreeInterpolationTimeline.IsPlaying())
				{
					FirstToThreeInterpolationTimeline.Stop();
				}
				SwitchThreePersonViewpoint();
				break;
			}
	}
}

void UUnrealCameraComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UUnrealCameraComponent::OpenCamera()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	if (CameraViewWidget)
	{
		CameraViewWidget->AddToViewport();
	}

	if (PlayerCharacter)
	{
		// 设置玩家摄像机输入映射
		if (PlayerController)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(CameraModeInputMappingContext, 0);
			}
		}
	}
}

void UUnrealCameraComponent::CloseCamera()
{
	CameraViewWidget->RemoveFromViewport();

	// 关闭摄像头，恢复玩家输入映射
	if (PlayerCharacter)
	{
		if (PlayerController)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(PlayerCharacter->ShootGameInputMappingContext, 0);
			}
		}
	}
}

void UUnrealCameraComponent::Shot()
{
	if (!CameraViewWidget->IsInViewport())
	{
		// 不在视口不允许执行摄像
		return;
	}

	if (FlashTimeline.IsPlaying())
	{
		// 通过闪光灯处理时间轴来判断是否完成上一次摄像
		return;
	}
	
	FlashTimeline.PlayFromStart();
	if (PlayerController)
	{
		switch (CameraShotMode)
		{
			case ECameraShotMode::Shot:
				{
					UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), ShotConsoleCommand, PlayerController);
					break;
				}
			case ECameraShotMode::HighResShot:
				{
					UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), HighResShotConsoleCommand, PlayerController);
					break;
				}
			case ECameraShotMode::CustomShot:
				{
					CustomShot();
					break;
				}
			default:
				{
					break;
				};
		}
		
	}
}

void UUnrealCameraComponent::CustomShot()
{
	if (PlayerController)
	{
		SaveScreenShot(GEngine->GameViewport->Viewport);
	}
}

// Called when the game starts
void UUnrealCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* Owner = GetOwner())
	{
		if (ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(Owner))
		{
			PlayerCharacter = OwnerCharacter;
			PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

			SetCameraViewPoint(ViewPointMode);

			// 绑定第一人称 -> 第三人称过渡
			if (FirstToThreeInterpolationCurve)
			{
				OnStartFirstToThreeInterpolation.BindDynamic(this, &ThisClass::UUnrealCameraComponent::FirstToThreeInterpolationHandle);
				FirstToThreeInterpolationTimeline.AddInterpFloat(FirstToThreeInterpolationCurve, OnStartFirstToThreeInterpolation);
			}

			if (ShotFlashCurve)
			{
				OnShotFlash.BindDynamic(this, &ThisClass::ShotFlashHandle);
				FlashTimeline.AddInterpFloat(ShotFlashCurve, OnShotFlash);
			}

			CameraViewWidget = CreateWidget<UCameraWidget>(PlayerController, CameraWidgetClass, TEXT("CameraViewWidget"));

			BindCameraInput();
		}
	}
	// ...
}

void UUnrealCameraComponent::SwitchFirstPersonViewpoint()
{
	if (PlayerCharacter && PlayerController)
	{
		PlayerController->SetViewTarget(PlayerCharacter);
		if (const USkeletalMeshSocket* FirstPersonSocket = PlayerCharacter->GetMesh()->GetSocketByName(FirstPersonSocketName))
		{
			const FVector FirstPerViewpointLocation = FirstPersonSocket->GetSocketLocation(PlayerCharacter->GetMesh());

			GetSpringArmComponent()->SetWorldLocation(FirstPerViewpointLocation);
			GetSpringArmComponent()->TargetArmLength = DefaultFirstTargetArmLength;
			GetSpringArmComponent()->TargetOffset = FVector(0,0,0);

			GetSpringArmComponent()->bInheritRoll = true;
			GetSpringArmComponent()->bInheritYaw = true;
			GetSpringArmComponent()->bInheritPitch = true;
			GetSpringArmComponent()->bUsePawnControlRotation = false;
		}
	}
}

void UUnrealCameraComponent::SwitchThreePersonViewpoint()
{
	if (PlayerCharacter && PlayerController)
	{
		PlayerController->SetViewTarget(PlayerCharacter);
		if (const USkeletalMeshSocket* ThreePersonSocket = PlayerCharacter->GetMesh()->GetSocketByName(ThreePersonSocketName))
		{
			const FVector ThreePerViewpointLocation = ThreePersonSocket->GetSocketLocation(PlayerCharacter->GetMesh());

			GetSpringArmComponent()->SetWorldLocation(ThreePerViewpointLocation);
			GetSpringArmComponent()->TargetArmLength = DefaultThreeTargetArmLength;
			GetSpringArmComponent()->TargetOffset = FVector(0,0,0);

			GetSpringArmComponent()->bInheritRoll = true;
			GetSpringArmComponent()->bInheritYaw = true;
			GetSpringArmComponent()->bInheritPitch = true;
			GetSpringArmComponent()->bUsePawnControlRotation = false;
		}
	}
}

void UUnrealCameraComponent::SwitchDebugRightViewpoint()
{
	if (PlayerController)
	{
		GetSpringArmComponent()->SetWorldRotation(RightDebugViewPointRotator);
		GetSpringArmComponent()->SetWorldLocation(this->GetComponentLocation());
		GetSpringArmComponent()->TargetArmLength = DefaultRightViewPointTargetArmLength;
		GetSpringArmComponent()->TargetOffset = FVector(0,0,0);

		GetSpringArmComponent()->bInheritRoll = false;
		GetSpringArmComponent()->bInheritYaw = false;
		GetSpringArmComponent()->bInheritPitch = false;
		GetSpringArmComponent()->bUsePawnControlRotation = false;
	}
}

void UUnrealCameraComponent::SwitchDebugTopViewPoint()
{
	if (PlayerController)
	{
		GetSpringArmComponent()->SetWorldRotation(TopDebugViewPointRotator); // 当不再使用继承父类而来的旋转时，只能通过调整弹簧臂的旋转来旋转弹簧臂末端的摄像机位置
		GetSpringArmComponent()->SetWorldLocation(this->GetComponentLocation()); // 重置弹簧臂的位置，与父类组件位置保持一致
		GetSpringArmComponent()->TargetArmLength = DefaultTopViewPointTargetArmLength; // 重置弹簧臂长度
		GetSpringArmComponent()->TargetOffset = FVector(0,0,0); // 重置弹簧臂的目标偏移值

		// 不再使用外部数据控制弹簧臂旋转
		GetSpringArmComponent()->bInheritRoll = false;
		GetSpringArmComponent()->bInheritYaw = false;
		GetSpringArmComponent()->bInheritPitch = false;
		GetSpringArmComponent()->bUsePawnControlRotation = false;
	}
}

void UUnrealCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FirstToThreeInterpolationTimeline.TickTimeline(DeltaTime);
	FlashTimeline.TickTimeline(DeltaTime);

	if (bIsShowCameraDraw)
	{
		const FVector OriginLocation = GetComponentLocation();
		const FVector SpringLocation = GetSpringArmComponent()->GetComponentLocation();
		const FVector CameraLocation = GetCameraComponent()->GetComponentLocation();

		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), SpringLocation, CameraLocation, ArrowSize, FLinearColor::Red);
		DrawDebugSphere(GetWorld(), OriginLocation, DebugSpereRadius, DebugSpereSegment, FColor::Green);
		DrawDebugSphere(GetWorld(), SpringLocation, DebugSpereRadius+5, DebugSpereSegment, FColor::Red);
		DrawDebugSphere(GetWorld(), CameraLocation, DebugSpereRadius+10, DebugSpereSegment, FColor::Yellow);
	}
}

void UUnrealCameraComponent::FirstToThreeInterpolationHandle(float InterpolationValue)
{
	const USkeletalMeshSocket* FirstPersonSocket = PlayerCharacter->GetMesh()->GetSocketByName(FirstPersonSocketName);
	const USkeletalMeshSocket* ThreePersonSocket = PlayerCharacter->GetMesh()->GetSocketByName(ThreePersonSocketName);
	if (FirstPersonSocket && ThreePersonSocket)
	{
		const FVector FirstViewpointLocation = FirstPersonSocket->GetSocketLocation(PlayerCharacter->GetMesh());
		const FVector ThreeViewpointLocation = ThreePersonSocket->GetSocketLocation(PlayerCharacter->GetMesh());

		const float NewViewpointLocationX = FMath::Lerp(FirstViewpointLocation.X, ThreeViewpointLocation.X, FMath::Clamp(InterpolationValue, 0, 1));
		const float NewViewpointLocationY = FMath::Lerp(FirstViewpointLocation.Y, ThreeViewpointLocation.Y, FMath::Clamp(InterpolationValue, 0, 1));
		const float NewViewpointLocationZ = FMath::Lerp(FirstViewpointLocation.Z, ThreeViewpointLocation.Z, FMath::Clamp(InterpolationValue, 0, 1));

		const FVector NewViewpointLocation(NewViewpointLocationX, NewViewpointLocationY, NewViewpointLocationZ);
		const float NewArmLength = FMath::Lerp(GetSpringArmComponent()->TargetArmLength, DefaultThreeTargetArmLength, FMath::Clamp(InterpolationValue, 0, 1));
		GetSpringArmComponent()->TargetArmLength = FMath::Clamp(NewArmLength, DefaultFirstTargetArmLength, DefaultThreeTargetArmLength);

		UE_LOG(CameraLog, Log, TEXT("Current Viewpoint Location Value : %f"), FirstViewpointLocation.X);
		UE_LOG(CameraLog, Log, TEXT("Three Target Location Value : %f"), ThreeViewpointLocation.X);
		UE_LOG(CameraLog, Log, TEXT("NewViewpointLocation X Value : %f"), NewViewpointLocationX);
		
		if (AConsoleVariableActor::GetShowCameraDebugLog())
		{
			UE_LOG(CameraLog, Log, TEXT("Interpolation Value : %f"), InterpolationValue);
			UE_LOG(CameraLog, Log, TEXT("Target Arm Length Value : %f"), GetSpringArmComponent()->TargetArmLength);
			UE_LOG(CameraLog, Log, TEXT("Current Viewpoint Location Value : %s"), *FirstViewpointLocation.ToString());
			UE_LOG(CameraLog, Log, TEXT("Three Target Location Value : %s"), *ThreeViewpointLocation.ToString());
			UE_LOG(CameraLog, Log, TEXT("First -> Three Interpolation : %s"), *NewViewpointLocation.ToString());
		}
		this->SetWorldLocation(NewViewpointLocation);
	}
}

void UUnrealCameraComponent::ShotFlashHandle(float Value)
{
	float NewFlash = FMath::Clamp(Value, 0, 1);

	const float MaskValue = NewFlash;
	CameraViewWidget->UpdateFlash(NewFlash, MaskValue);
}

void UUnrealCameraComponent::EnterDebugModePressed()
{
	bIsDebuging = true;
}

void UUnrealCameraComponent::LeaveDebugModeRelease()
{
	bIsDebuging = false;
}

void UUnrealCameraComponent::SelectViewPoint1Pressed()
{
	if (!bIsDebuging)
	{
		return;
	}
	SetCameraViewPoint(CameraViewPointModes[0]);
}

void UUnrealCameraComponent::SelectViewPoint2Pressed()
{
	if (!bIsDebuging)
	{
		return;
	}
	SetCameraViewPoint(CameraViewPointModes[1]);
}

void UUnrealCameraComponent::SelectViewPoint3Pressed()
{
	if (!bIsDebuging)
	{
		return;
	}
	SetCameraViewPoint(CameraViewPointModes[2]);
}

void UUnrealCameraComponent::SelectViewPoint4Pressed()
{
	if (!bIsDebuging)
	{
		return;
	}
	SetCameraViewPoint(CameraViewPointModes[3]);
}

void UUnrealCameraComponent::ResetPressed()
{
	if (!bIsDebuging)
	{
		return;
	}
	SetCameraViewPoint(ViewPointMode);
}

void UUnrealCameraComponent::AdjustArmLength(const FInputActionValue& ActionValue)
{
	if (!bIsDebuging)
	{
		return;
	}
	float CurrentArmLength = GetSpringArmComponent()->TargetArmLength;
	GetSpringArmComponent()->TargetArmLength = CurrentArmLength + (ActionValue.GetMagnitude() * ArmLengthSpeed);
}

void UUnrealCameraComponent::AdjustXOffset(const FInputActionValue& ActionValue)
{
	FVector CurrentTargetOffset = GetSpringArmComponent()->TargetOffset;
	CurrentTargetOffset.X += ActionValue.GetMagnitude() * ArmTartOffsetXSpeed;
	GetSpringArmComponent()->TargetOffset = CurrentTargetOffset;
}

void UUnrealCameraComponent::AdjustYOffset(const FInputActionValue& ActionValue)
{
	FVector CurrentTargetOffset = GetSpringArmComponent()->TargetOffset;
	CurrentTargetOffset.Y += ActionValue.GetMagnitude() * ArmTartOffsetYSpeed;
	GetSpringArmComponent()->TargetOffset = CurrentTargetOffset;
}

void UUnrealCameraComponent::AdjustZOffset(const FInputActionValue& ActionValue)
{
	FVector CurrentTargetOffset = GetSpringArmComponent()->TargetOffset;
	CurrentTargetOffset.Z += ActionValue.GetMagnitude() * ArmTartOffsetZSpeed;
	GetSpringArmComponent()->TargetOffset = CurrentTargetOffset;
}

void UUnrealCameraComponent::ZoomFOV(const FInputActionValue& ActionValue)
{
	float CurrentFOV = GetCameraComponent()->FieldOfView;

	// 滚轮输入的值
	if (AConsoleVariableActor::GetShowCameraDebugLog())
	{
		UE_LOG(CameraLog, Log, TEXT("CurrentFOV : %f"), CurrentFOV);
		UE_LOG(CameraLog, Log, TEXT("Zoom Value: %f"), ActionValue.GetMagnitude());
	}

	float NewFOV = CurrentFOV + (ZoomSpeed * ActionValue.GetMagnitude());

	NewFOV = FMath::Clamp(NewFOV, MinFOV, MaxFOV);
	GetCameraComponent()->FieldOfView = NewFOV;
}

void UUnrealCameraComponent::SaveScreenShot(FViewport* InViewport)
{
	FVector2d ViewpointSize = InViewport->GetSizeXY();
	int32 X = ViewpointSize.X;
	int32 Y = ViewpointSize.Y;
	FIntRect InRect(0, 0, X, Y);

	// 未压缩截图数据
	TArray<FColor> RawData;

	// 读取视口数据
	GetViewportScreenShot(InViewport, RawData, InRect);
	// 截图照片处理
	TArray<uint8> ImageData;
	FImageUtils::CompressImageArray(InRect.Width(), InRect.Height(), RawData, ImageData);

	// 截图照片处理
	FString ImagePath = FString::Printf(TEXT("%s%s%f%s"), *FPaths::GameSourceDir(), TEXT("Shot/"), GetWorld()->GetTimeSeconds(),TEXT(".jpg"));
	UE_LOG(CameraLog, Log, TEXT("%s"), *ImagePath)

	// 保存文件
	FFileHelper::SaveArrayToFile(ImageData, *ImagePath);
}

void UUnrealCameraComponent::CreateShotTexture()
{
	// 存储截图文件名
	TArray<FString> Files;
	// 存储截图的文件夹绝对路径
	FString ShotFilesParentPath = FPaths::ConvertRelativePathToFull(FString::Printf(TEXT("%s%s"), *FPaths::GameSourceDir(), TEXT("Shot/")));

	FPaths::NormalizeDirectoryName(ShotFilesParentPath);
	IFileManager& FileManager = IFileManager::Get();
	
	if (FileManager.DirectoryExists(*ShotFilesParentPath))
	{
		FileManager.FindFiles(Files, *ShotFilesParentPath, TEXT(".jpg"));
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("待处理图片数量：%d"), Files.Num()));
		for (int I=0; I < Files.Num(); I++)
		{
			TArray<uint8> RawFileData;
			FString FileName = ShotFilesParentPath + "/" + Files[I];
			if (FFileHelper::LoadFileToArray(RawFileData, *FileName))
			{
				// 每个截图文件一个 Package
				/*
				 * Package 的位置由 MountPoint 开始，这个 MountPoint 就是类似 /Game、/Engine 这样的东西，如下：GameDir
				 *
				 * 如果我们需要 在指定目录保存这些文件，比如我们不想在 /Game、/Engine 的目录写入这些信息，想在自己的插件目录、其它本地目录保存，
				 * 那么我们需要先注册 MountPoint(FPackageName::RegisterMountPoint("/你想的 MountPoint 名字/",  对应的路径); - 注册 MountPoint 失败 TODO)
				 *
				 * note：AssetPath 很重要，否则保存失败
				 */

				// 默认使用 /Game
				FString GameDir = TEXT("/Game");
				FString PackageName = "/UnrealGame/ShotTexture";
				FString AssetPath = GameDir + PackageName+ TEXT("/");
				// 纹理资产名称
				FString TextureName = FString::Printf(TEXT("T_%d"), UKismetMathLibrary::RandomInteger(100000));
				AssetPath += TextureName;

				UPackage* Package = CreatePackage(*AssetPath);
				Package->FullyLoad();
				
				IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
				
				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
				
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
				{
					TArray<uint8> UncompressedRGBA;
					if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRGBA))
					{
						int32 SizeX = ImageWrapper->GetWidth();
						int32 SizeY = ImageWrapper->GetHeight();
						const EPixelFormat PixelFormat = PF_B8G8R8A8;
						int32 PixelSize = SizeX * SizeY;

						// Create new texture pointer           
						UTexture2D* Texture = NewObject<UTexture2D>(Package, FName(*TextureName), RF_Public | RF_Standalone | RF_MarkAsRootSet);

						// 写入纹理数据
						Texture->SetPlatformData(new FTexturePlatformData());
						Texture->GetPlatformData()->SizeX = SizeX;
						Texture->GetPlatformData()->SizeY = SizeY;
						Texture->GetPlatformData()->SetNumSlices(1);
						Texture->GetPlatformData()->PixelFormat = PixelFormat;

						// Determine whether it is a power of 2 to use mipmap
						if ( (SizeX & (SizeX - 1) || (SizeY & (SizeY - 1))) )
							Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

						// Allocate first mipmap.
						FTexture2DMipMap* Mip = new FTexture2DMipMap();
						Texture->GetPlatformData()->Mips.Add(Mip);
						Mip->SizeX = SizeX;
						Mip->SizeY = SizeY;

						// Lock the texture so that it can be modified - 锁定 Texture 让它可以被修改
						Mip->BulkData.Lock(LOCK_READ_WRITE);
						uint8* TextureData = (uint8*) Mip->BulkData.Realloc(PixelSize);
						FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), PixelSize);
						Mip->BulkData.Unlock();

						//通过以上步骤，我们完成数据的临时写入
						//执行完以下这两个步骤，编辑器中的 asset 会显示可以保存的状态（如果是指定 Asset 来获取 UTexture2D 的指针的情况下
						Texture->Source.Init(SizeX, SizeY, 1, 1, ETextureSourceFormat::TSF_BGRA8, UncompressedRGBA.GetData());
						Texture->UpdateResource();

						Package->MarkPackageDirty();

						// 创建 Asset 并清理无用数据
						FAssetRegistryModule::AssetCreated(Texture);
						//通过 asset 路径获取包中文件名
						FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
						//进行保存
						FSavePackageArgs SavePackageArgs;
						bool bIsSave = UPackage::SavePackage(Package, Texture, *PackageFileName, SavePackageArgs);
						if (bIsSave)
						{
							GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::Printf(TEXT("成功生成纹理资产：%s"), *PackageFileName));
						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("生成纹理资产失败。")));
						}
					}
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("指定路径文件不存在，路径：[%s]"),  *ShotFilesParentPath));
	}
}