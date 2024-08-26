// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TicTacToeGameModeBase.generated.h"

UENUM(BlueprintType)
enum class ETurnState :uint8
{
	Player1 = 0,	// �������
	Player2 = 1,	// �Է�
	None = 2,
};

UENUM(BlueprintType)
enum class EResultState :uint8
{
	Win = 0,		// ʤ��
	NoMore = 1,		// ƽ��
	Loss = 2,		// ʧ�ܣ�����ʱû���ã����������ʱ������
	PlayerNext = 3	// ��һ��������ʱ���ã��������ʱ����
};

/**
 * 
 */
UCLASS()
class TICTACTOE_API ATicTacToeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ATicTacToeGameModeBase();

public:
	//DECLARE_DELEGATE_TwoParams(FOnCellStateChanged, ETurnState /*Player*/, int /*Id*/);

	struct CellInfo
	{
		uint8 CellNumber;
		uint8 CellStepMin;		// ����߷�
		uint8 CellStepMax;		// ����߷�
		EResultState CellResult;
		TArray<TUniquePtr<CellInfo>> Strategy;		// ��������
	};

	UFUNCTION(BlueprintCallable)
	ETurnState GetCurrentPlayer()const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerMode(bool RobotMode);

	UFUNCTION(BlueprintCallable)
	bool IsRobotMode()const;

	UFUNCTION(BlueprintCallable)
	void TogglePlayer(bool Player1);

	UFUNCTION(BlueprintCallable)
	EResultState PlayerPlay(int Id);

	UFUNCTION(BlueprintCallable)
	int GetRobotNum(int DefaultId = -1);

	UFUNCTION(BlueprintCallable)
	void GameInit();

	UFUNCTION(BlueprintPure)
	EResultState CheckRobotResult(int Id);

	UFUNCTION(BlueprintPure)
	EResultState CheckPlayerResult(int Id);

private:
	uint8 Calculator(TUniquePtr<CellInfo>& Cell, uint8 Number, bool Player);
	bool CheckWin()const;
	bool CheckNoMore()const;
	int Pending(const TUniquePtr<CellInfo>& Node)const;

private:
	static const uint8 MaxSize{ 10 };
	static const uint8 MaxStep{ 128 };
	ETurnState ChessBoard[MaxSize];
	TUniquePtr<CellInfo> Strategy;
	TUniquePtr<CellInfo> CurrentCell;
	TUniquePtr<CellInfo> NextCell;
	bool bCurrentPlayer;
	bool bRobotMode;
};
