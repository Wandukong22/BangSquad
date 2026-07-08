# BangSquad

세션 생성부터 로비, 스테이지, 미니게임까지 멀티플레이 상태 일관성을 유지하는  
**Listen Server 기반 4인 협동 액션 RPG**입니다.

<p align="center">
  <a href="YOUTUBE_LINK">
    <img src="THUMBNAIL_OR_GIF_PATH" alt="BangSquad Gameplay" width="820">
  </a>
</p>

## 프로젝트 개요

- 장르: 3D 협동 액션 RPG
- 개발 기간: 2025.12.22 ~ 2026.03.06
- 개발 인원: 개발 4명 / 기획 1명
- 사용 엔진: Unreal Engine 5
- 사용 언어: C++
- 플랫폼: Windows PC
- 네트워크 구조: Listen Server
- 담당 역할: 멀티플레이 세션, 로비 상태 동기화, 직업 선택, 스테이지 전환, 체크포인트, 리스폰, 관전, 미니게임 순위 계산

## 플레이 영상 또는 스크린샷

- 플레이 영상: [YouTube](YOUTUBE_LINK)
- 기술 문서: [PDF](PORTFOLIO_PDF_LINK)
- 저장소: [GitHub](REPOSITORY_LINK)

## 게임 설명

**BangSquad**는 4명의 플레이어가 서로 다른 직업을 선택하고,  
협력하여 스테이지, 미니게임, 보스전을 돌파하는 멀티플레이 액션 RPG입니다.

이 프로젝트에서 저는 게임플레이 전체보다  
**세션 생성부터 로비, 스테이지, 미니게임까지 이어지는 멀티플레이 구조와 상태 동기화 흐름**을 중심으로 구현했습니다.

## 주요 기능

- OnlineSubsystem 기반 멀티플레이 세션 생성, 검색, 참가
- LAN / Steam 환경 분기
- RepNotify 기반 로비 Phase 동기화
- 서버 권한 기반 직업 선택 및 중복 방지
- Portal 기반 스테이지 전환
- 체크포인트 및 게임 모드별 리스폰 시스템
- 사망 후 관전 전환 처리
- `ISaveInterface` 기반 오브젝트 상태 저장 및 복원
- 미니게임 실시간 순위 계산
- Arena Phase 상태 머신 구현

## 조작법

> 실제 프로젝트 Input Mapping에 맞게 수정해 주세요.

- 이동: `WASD`
- 시점 이동: `Mouse`
- 기본 공격: `좌클릭`
- 스킬: `프로젝트 설정 입력`
- 상호작용: `프로젝트 설정 입력`
- 관전 대상 변경: `프로젝트 설정 입력`

## 개발 과정

1. Gameplay Framework 기준으로 상태 책임 분리
2. OnlineSubsystem 기반 세션 생성 / 검색 / 참가 구현
3. 로비 Phase와 직업 선택 구조 구현
4. Portal 기반 스테이지 전환과 DataAsset 맵 참조 구성
5. 체크포인트, 리스폰, 관전 시스템 구현
6. 미니게임 순위 계산과 Arena 상태 머신 구현
7. 멀티클라이언트 테스트와 예외 상황 수정

## 기술적으로 신경 쓴 부분

- 서버가 최종 상태를 결정하도록 구조를 설계했습니다.
- `GameMode`, `GameState`, `PlayerState`, `PlayerController`, `GameInstance`의 책임을 분리했습니다.
- 로비 Phase처럼 현재 상태 복원이 필요한 데이터는 RPC 대신 `RepNotify`로 복제했습니다.
- 세션 검색 결과를 UI 전용 DTO로 변환해 네트워크 로직과 UI를 분리했습니다.
- `StageIndex + Section` 기준으로 맵과 리소스를 조회하도록 DataAsset 구조를 구성했습니다.
- 저장 시스템이 특정 Actor 타입에 의존하지 않도록 `ISaveInterface` 기반으로 설계했습니다.

## 문제 해결 경험

### 문제 1. 직업 선택 Race Condition

- 원인: 클라이언트 UI 비활성화만으로는 동시 요청을 막을 수 없었음
- 해결: `LobbyGameMode`에서 서버 권한으로 최종 검증하고 `LobbyGameState` / `LobbyPlayerState`로 상태를 분리 관리
- 배운 점: 멀티플레이에서는 UI 상태가 아니라 서버가 최종 상태를 확정해야 함

### 문제 2. Seamless Travel 중 Lobby UI Access Violation

- 원인: 이전 World에서 등록한 반복 Timer 콜백이 Travel 이후에도 실행되며 유효하지 않은 객체를 참조함
- 해결: 반복 Timer를 제거하고 새로운 World의 `BeginPlay()`에서 UI를 직접 초기화하도록 변경
- 배운 점: `IsValid()`만이 아니라 Travel 이후 객체 생명주기와 World 전환 시점을 함께 고려해야 함

### 문제 3. Replication 초기화 순서 문제

- 원인: `RisingPlatform`에서 복제 상태가 적용될 때 `EndLocation`이 아직 계산되지 않아 잘못된 위치가 적용됨
- 해결: `BeginPlay()`에서 기준 위치를 먼저 계산한 뒤 저장/복제 상태를 반영하도록 순서를 변경
- 배운 점: 네트워크 상태 적용 전에 필요한 초기화 데이터가 먼저 준비되어 있어야 함

## AI 활용 내용

이 프로젝트에서는 AI를 다음 작업에 활용했습니다.

- 문제 원인 후보 정리
- 네트워크 구조 검토
- 트러블슈팅 정리
- 코드 초안 및 예외 처리 점검
- README와 기술 문서 초안 작성

AI가 제안한 내용은 바로 적용하지 않고,  
프로젝트 구조에 맞는지 검토한 뒤 멀티클라이언트 테스트로 확인했습니다.

## 실행 방법

1. 저장소를 Clone합니다.
2. `uproject` 파일로 프로젝트 파일을 생성합니다.
3. Rider 또는 Visual Studio에서 빌드합니다.
4. 프로젝트를 실행한 뒤 Host 또는 Join으로 멀티플레이를 테스트합니다.

## 코드 참고

- [BSGameInstance](./Source/Project_Bang_Squad/Core/BSGameInstance.cpp)
- [LobbyGameMode](./Source/Project_Bang_Squad/Game/Lobby/LobbyGameMode.cpp)
- [LobbyGameState](./Source/Project_Bang_Squad/Game/Lobby/LobbyGameState.cpp)
- [MapPortal](./Source/Project_Bang_Squad/Game/Stage/MapPortal.cpp)
- [StageGameMode](./Source/Project_Bang_Squad/Game/Stage/StageGameMode.cpp)
- [MiniGamePlayerState](./Source/Project_Bang_Squad/Game/MiniGame/MiniGamePlayerState.cpp)
- [MiniGameWidget](./Source/Project_Bang_Squad/UI/MiniGame/MiniGameWidget.cpp)
- [ArenaGameState](./Source/Project_Bang_Squad/Game/MiniGame/ArenaGameState.cpp)

## 회고

이 프로젝트를 통해 멀티플레이 게임에서는 기능 구현 자체보다  
**상태를 누가 소유하고, 누가 변경하며, 어떤 시점에 동기화할지 결정하는 구조 설계**가 중요하다는 점을 배웠습니다.

특히 서버 권한 직업 선택, Seamless Travel 생명주기 충돌, Replication 초기화 순서 문제를 해결하면서  
Unreal Engine의 Gameplay Framework와 멀티플레이 상태 동기화를 실제 프로젝트 단위로 경험할 수 있었습니다.
