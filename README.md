# BangSquad

> 역할을 나누어 스테이지와 기믹을 돌파하는  
> **Listen Server 기반 4인 협동 액션 RPG**

<p align="center">
  <img src="./Docs/Images/BangSquad_Main.png" alt="BangSquad 대표 이미지" width="800">
</p>

<p align="center">
  <a href="플레이영상_URL">플레이 영상</a>
  ·
  <a href="기술문서_URL">기술문서</a>
  ·
  <a href="빌드다운로드_URL">빌드 다운로드</a>
</p>

---

## 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 게임명 | **BangSquad** |
| 프로젝트 유형 | 기업협약 팀 프로젝트 |
| 장르 | 3D 협동 액션 RPG |
| 개발 인원 | 개발 4명 / 기획 1명 |
| 개발 기간 | 2025.12.22 ~ 2026.03.06 |
| 개발 환경 | Unreal Engine 5, Rider, GitHub |
| 사용 기술 | C++, OnlineSubsystem, Replication, RPC, DataAsset, Gameplay Framework |
| 담당 기능 | 멀티플레이 세션, 로비, 스테이지 전환, 리스폰, 체크포인트, 관전, 미니게임 |

---

## 게임 소개

BangSquad는 4명의 플레이어가 서로 다른 직업을 선택하고 협력하여  
스테이지, 미니게임, 보스전을 돌파하는 멀티플레이 액션 RPG입니다.

### 핵심 플레이 흐름

```text
메인 메뉴
  ↓
세션 생성 / 검색 / 참가
  ↓
로비 입장
  ↓
직업 체험 및 Ready
  ↓
직업 확정
  ↓
Stage 1 ~ 3 진행
  ↓
미니게임 또는 보스전
  ↓
엔딩
```

### 주요 특징

- Listen Server 기반 4인 멀티플레이
- 로비 → 스테이지 → 미니게임 → 보스전 구조
- 서버 권한 기반 직업 선택 및 중복 방지
- 플레이어별 상태와 공용 상태를 분리한 동기화 구조
- 체크포인트, 리스폰, 관전 시스템
- 실시간 미니게임 순위 계산
- 게임 모드별 독립적인 진행 규칙

---

## 담당 기능

### 1. OnlineSubsystem 기반 멀티플레이 세션

세션 생성, 검색, 참가 흐름을 `GameInstance`에서 관리했습니다.

```text
Host
세션 생성
  → OnCreateSessionComplete
  → ServerTravel
  → 로비 입장

Join
세션 검색
  → OnFindSessionsComplete
  → 서버 목록 갱신
  → 세션 선택
  → JoinSession
  → OnJoinSessionComplete
  → ClientTravel
  → 로비 입장
```

#### 구현 내용

- `CreateSession`, `FindSessions`, `JoinSession` 구현
- OnlineSubsystem 비동기 완료 Delegate 기반 후속 처리
- 세션 완료 이전 Travel 실행을 방지
- 세션 검색 결과를 서버 목록 UI에 반영
- Listen Server 기반 로비 진입 처리

#### 설계 포인트

OnlineSubsystem API는 비동기로 동작하므로, 함수 호출 직후 결과를 사용하지 않고  
각 완료 Delegate에서만 Travel과 UI 갱신을 수행하도록 구성했습니다.

---

### 2. LAN / Steam 환경 분리

로컬 테스트 환경과 Steam 배포 환경에서 동일한 세션 흐름을 사용할 수 있도록  
OnlineSubsystem 종류에 따라 `SessionSettings`만 분기했습니다.

| 설정 | LAN | Steam |
|---|---:|---:|
| `bIsLANMatch` | true | false |
| `bUsesPresence` | false | true |
| 검색 방식 | Broadcast | Presence |
| 연결 범위 | 같은 네트워크 | Steam 친구 / Presence |

#### 결과

- Steam 실행 없이 LAN 멀티플레이 테스트 가능
- Steam 환경에서는 Presence 기반 검색과 친구 초대 지원
- 세션 생성·검색·참가 로직은 공통으로 유지
- 테스트 환경과 배포 환경의 코드 중복 최소화

---

### 3. 서버 목록 UI 연동

OnlineSubsystem의 검색 결과 타입을 UI에서 직접 사용하지 않도록  
표시용 데이터 구조체로 변환해 전달했습니다.

```text
FOnlineSessionSearchResult
          ↓ 변환
      FServerData
          ↓ 전달
        Widget
```

```cpp
struct FServerData
{
    FString Name;
    uint16 CurrentPlayers;
    uint16 MaxPlayers;
    FString HostUserName;
};
```

#### 설계 목적

- 세션 검색 로직과 UI 의존성 분리
- UI에는 표시용 데이터만 전달
- OnlineSubsystem 내부 타입의 직접 노출 방지
- 서버 목록 UI 변경 시 세션 코드 수정 범위 축소

---

### 4. Gameplay Framework 기반 상태 관리

멀티플레이 상태를 책임에 따라 분리했습니다.

| 클래스 | 역할 |
|---|---|
| `GameMode` | 서버 권한 기반 규칙 처리, 스폰, 리스폰, 스테이지 전환 |
| `GameState` | 전체 플레이어가 공유하는 상태 복제 |
| `PlayerState` | 플레이어별 직업, 체크포인트, 진행도, 순위 저장 |
| `PlayerController` | 입력, UI, 서버 RPC, 관전 전환 |
| `GameInstance` | 맵 전환 후에도 필요한 로컬 캐시와 세션 관리 |

#### 상태 분리 원칙

- **개인 상태**: PlayerState
- **공용 상태**: GameState
- **규칙 판정**: GameMode
- **입력 및 UI**: PlayerController
- **맵 전환 간 유지 데이터**: GameInstance

---

### 5. 로비 Phase 동기화

로비 흐름을 Phase 기반 상태 머신으로 구성했습니다.

```text
PreviewJob
  ↓ 모든 플레이어 Ready
SelectJob
  ↓ 모든 플레이어 직업 확정
GameStarting
  ↓
Stage 1
```

#### 구현 방식

- 서버의 `LobbyGameMode`가 Phase 변경 조건 판단
- `LobbyGameState`의 `CurrentPhase` 변경
- `RepNotify`로 모든 클라이언트에 현재 Phase 복제
- `OnRep_CurrentPhase()`에서 Delegate Broadcast
- `LobbyPlayerController`가 Delegate를 구독해 UI 갱신

```cpp
UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
ELobbyPhase CurrentPhase;
```

#### RepNotify를 사용한 이유

RPC는 이벤트가 발생한 순간만 전달하므로 Late Join 플레이어가  
현재 로비 상태를 복원하기 어렵습니다.

반면 RepNotify는 현재 상태값 자체를 복제하므로  
중간에 참가한 플레이어도 현재 Phase에 맞는 UI를 표시할 수 있습니다.

---

### 6. 서버 권한 기반 직업 선택

직업 선택 요청은 클라이언트에서 시작하지만, 최종 확정은 서버에서만 수행합니다.

```text
직업 확정 요청
  → LobbyPlayerController
  → Server RPC
  → LobbyGameMode에서 검증
  → LobbyGameState의 직업 점유 상태 갱신
  → LobbyPlayerState에 확정 직업 저장
  → GameInstance에 로컬 복원용 캐시 저장
```

#### 데이터 역할

- `LobbyGameState`: 전체 직업 점유 상태
- `LobbyPlayerState`: 플레이어별 확정 직업
- `GameInstance`: 맵 전환 후 로컬 UI 복원용 캐시

---

### 7. Race Condition 해결

#### 문제

클라이언트 UI 버튼을 비활성화하는 방식만으로는  
패킷이 서버에 도착하기 전에 두 플레이어가 동시에 같은 직업을 요청할 수 있었습니다.

#### 기존 방식의 한계

- UI 비활성화는 클라이언트 로컬 처리
- 서버의 실제 직업 점유 상태를 보장하지 못함
- 동시에 요청한 경우 중복 확정 가능
- 직업 변경 시 기존 직업과 신규 직업 상태 관리가 복잡해짐

#### 해결

- 전체 직업 점유 상태를 `LobbyGameState`에서 관리
- 플레이어별 확정 직업은 `LobbyPlayerState`에서 별도 관리
- `LobbyGameMode::TryConfirmJob()`에서 서버 권한으로 최종 검증
- 이미 확정한 플레이어가 직업을 변경하면 기존 직업 점유를 먼저 해제
- 검증 성공 시에만 새로운 직업을 확정

```cpp
bool ALobbyGameMode::TryConfirmJob(...)
{
    if (!GS->IsJobAvailable(Job))
    {
        return false;
    }

    if (RequestingPS->GetIsConfirmedJob())
    {
        const EJobType OldJob = RequestingPS->GetSavedJobType();
        GS->RemoveTakenJob(OldJob);
    }

    GS->AddTakenJob(Job);
    RequestingPS->SetSavedJobType(Job);
    RequestingPS->SetIsConfirmedJob(true);

    return true;
}
```

#### 결과

동시에 같은 직업을 요청하더라도  
서버에서 먼저 검증된 한 명만 해당 직업을 확정하도록 처리했습니다.

---

### 8. DataAsset 기반 스테이지 전환

스테이지 이동 정보를 코드에 직접 작성하지 않고 `MapDataAsset`으로 관리했습니다.

```text
포탈 진입
  → 현재 진입 인원 확인
  → 전원 집결
  → 카운트다운 시작
  → Stage Index / Section으로 이동 대상 결정
  → DataAsset에서 Level 조회
  → ServerTravel
```

#### DataAsset에 저장한 정보

- Stage Index
- Section
- 이동할 Level
- 표시 이름
- 로딩 이미지
- 미니게임 결과 이미지

#### DataAsset을 사용한 이유

Level과 Texture 같은 Asset 참조가 필요했고,  
스테이지 추가 및 변경 시 코드 수정 없이 데이터를 편집할 수 있도록 하기 위해  
DataTable 대신 DataAsset으로 구성했습니다.

---

### 9. 객체 상태 저장 및 복원

미니게임 진입 후 기존 스테이지로 복귀할 때  
퍼즐과 오브젝트 상태가 초기화되는 문제를 해결했습니다.

#### 저장 흐름

```text
ISaveInterface 구현 객체 수집
  → SaveActorData() 호출
  → SaveID 기준으로 GameInstance에 저장
  → 스테이지 재진입
  → 저장 데이터 조회
  → LoadActorData() 호출
```

```cpp
struct FActorSaveData
{
    FTransform ActorTransform;
    float FloatData;
    bool BoolData;
};
```

#### 설계 특징

- 저장 대상은 `ISaveInterface` 구현 여부로 판별
- 저장 시스템은 인터페이스 함수만 호출
- 각 객체가 자신의 저장 데이터 구성을 담당
- SaveID 기반으로 객체별 상태 관리
- 새로운 저장 대상을 추가해도 기존 저장 시스템 코드 수정 최소화

---

### 10. 실시간 미니게임 순위

플레이어의 체크포인트 진행도와 다음 체크포인트까지의 거리를 조합해  
실시간 순위를 계산했습니다.

```text
체크포인트 도달 여부 확인
  → 현재 체크포인트 진행도 계산
  → 다음 체크포인트까지 거리 계산
  → 진행 점수 비교
  → 순위 정렬
  → 실시간 순위 UI 갱신
```

#### 진행 점수 기준

```text
진행 점수 = 체크포인트 진행도 + 거리 기반 진행도
```

#### 처리 방식

- 같은 체크포인트 구간에서도 거리로 순위를 구분
- 완주자는 도착 순위를 유지
- 미완주자는 최종 진행 점수로 순위 결정
- 순위 계산은 서버에서 수행
- 체크포인트와 순위 정보는 PlayerState로 동기화
- 최종 결과 UI만 클라이언트에 전달

---

### 11. Arena 미니게임 Phase 상태 머신

Arena 미니게임은 `GameState` 기반 4단계 Phase로 구성했습니다.

```text
Waiting
  ↓
Surviving
  ↓
FloorSinking
  ↓
Finished
```

| Phase | 처리 내용 |
|---|---|
| Waiting | 게임 시작 대기, 5초 카운트다운 |
| Surviving | 플레이어 생존전, 전장 축소까지 남은 시간 표시 |
| FloorSinking | 바닥 하강, 전장 축소 |
| Finished | 최후 생존자 결정, 결과 UI 출력 |

#### 설계 포인트

- `Surviving`과 `FloorSinking` 반복
- 최대 2회 전장 축소
- 플레이어 탈락 시점에 생존 순위 기록
- Phase는 GameState에 복제
- UI 반응은 PlayerController RPC로 분리
- 상태 관리와 UI 처리를 분리하여 미니게임 흐름을 독립적으로 유지

---

### 12. 모드별 리스폰 규칙

일반 스테이지, 미니게임, 보스전은 서로 다른 리스폰 규칙을 사용합니다.

| 구분 | 일반 스테이지 | 미니게임 | 보스전 |
|---|---|---|---|
| 리스폰 시간 | 사망 횟수에 따라 증가, 최대값 제한 | 고정 | 고정 |
| 리스폰 기준 | 개인 사망 횟수 | 개인 진행도 및 사망 상태 | 팀 부활 횟수 |
| 상태 관리 | PlayerState | PlayerState | GameState |
| 체크포인트 | 공용 | 개인 | 없음 |
| 부활 조건 | 대기시간 종료 | 대기시간 종료 | 팀 부활 횟수 잔여 |

#### 설계 원칙

- 게임 모드별 규칙은 각 GameMode에서 분리
- 개인 상태는 PlayerState에서 관리
- 팀 공유 상태는 GameState에서 관리
- 규칙 차이를 하나의 조건문에 누적하지 않고 모드별 정책으로 분리

---

### 13. 관전 및 체크포인트

#### 관전 흐름

```text
플레이어 사망
  → 생존 플레이어 탐색
  → 관전 대상 설정
  → 리스폰 시 관전 종료
```

#### 체크포인트 규칙

**일반 스테이지**

- 공용 체크포인트
- GameState에 저장
- 모든 플레이어가 같은 위치에서 리스폰

**미니게임**

- 개인 체크포인트
- PlayerState에 저장
- 플레이어별 진행 위치 유지

**보스전**

- 체크포인트 없음
- 팀 부활 횟수를 GameState에서 공유
- 남은 부활 횟수 기준으로 재도전

---

## 기술적 핵심

### 서버 권한 원칙

직업 확정, Phase 변경, 순위 계산, 리스폰 판정과 같은  
게임 결과에 영향을 주는 로직은 서버에서만 수행했습니다.

### 상태와 이벤트의 구분

- 지속적으로 복원되어야 하는 값: Replication / RepNotify
- 특정 시점에만 실행되는 동작: RPC
- UI와 게임 상태 연결: Delegate

### 상태 책임 분리

```text
GameMode        : 서버 규칙
GameState       : 공용 상태
PlayerState     : 개인 상태
PlayerController: 입력 / UI / 관전
GameInstance    : 세션 / 맵 전환 캐시
```

---

## 트러블슈팅 요약

| 문제 | 원인 | 해결 |
|---|---|---|
| 세션 완료 전 Travel 실행 | OnlineSubsystem 비동기 처리 | 완료 Delegate에서 Travel 수행 |
| Late Join 시 로비 UI 불일치 | RPC는 과거 이벤트 복원 불가 | Phase를 RepNotify로 복제 |
| 직업 중복 확정 | 클라이언트 UI만으로 동시 요청 차단 불가 | 서버 권한 최종 검증 |
| 미니게임 복귀 후 퍼즐 초기화 | 맵 전환 시 Actor 재생성 | ISaveInterface 기반 저장·복원 |
| 같은 체크포인트에서 순위 구분 불가 | 체크포인트 인덱스만 비교 | 다음 체크포인트까지 거리 추가 |
| 모드별 리스폰 로직 복잡화 | 서로 다른 규칙을 하나의 흐름에 누적 | GameMode별 정책 분리 |

---

## 개발 환경

```text
Engine      Unreal Engine 5
Language    C++
IDE         JetBrains Rider
VCS         Git / GitHub
Network     Listen Server
Online      OnlineSubsystem LAN / Steam
```

---

## 프로젝트 구조 예시

```text
Source/
└─ BangSquad/
   ├─ Core/
   │  ├─ BSGameInstance
   │  └─ MapDataAsset
   ├─ Lobby/
   │  ├─ LobbyGameMode
   │  ├─ LobbyGameState
   │  ├─ LobbyPlayerState
   │  └─ LobbyPlayerController
   ├─ Stage/
   │  ├─ Portal
   │  ├─ SaveInterface
   │  └─ CheckPoint
   ├─ MiniGame/
   │  ├─ ArenaGameMode
   │  ├─ ArenaGameState
   │  └─ RankingSystem
   └─ Respawn/
      ├─ RespawnGameMode
      └─ ObserverSystem
```

> 실제 프로젝트 폴더명에 맞게 수정해 주세요.

---

## 실행 방법

1. Unreal Engine 프로젝트를 실행합니다.
2. OnlineSubsystem 테스트 환경을 선택합니다.
   - 로컬 테스트: LAN
   - 배포 테스트: Steam
3. Host는 세션을 생성합니다.
4. 다른 플레이어는 Join 메뉴에서 세션을 검색하고 참가합니다.
5. 로비에서 Ready 및 직업 확정을 완료하면 게임이 시작됩니다.

> Steam 테스트 시 Steam 클라이언트 실행, App ID 설정, 서로 다른 계정 사용이 필요할 수 있습니다.

---

## 링크

- 플레이 영상: `추가 예정`
- 기술문서: `추가 예정`
- 빌드 다운로드: `추가 예정`
- GitHub Repository: `현재 저장소`

---

## 담당자

**정예원 — Game Client Programmer**

- 멀티플레이 세션
- 로비 및 직업 선택
- Stage 전환
- 체크포인트 및 객체 상태 저장
- 리스폰 및 관전
- 미니게임 진행 및 실시간 순위
