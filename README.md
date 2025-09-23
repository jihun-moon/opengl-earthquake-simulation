# 3D 지진 대피 시뮬레이션 (C++ · OpenGL · GLUT)

> ### 3줄 요약
>
>   - **3D 시뮬레이션**: C++과 OpenGL을 사용하여 캠퍼스(교실, 복도, 운동장) 공간에서의 지진 발생 상황을 시뮬레이션합니다.
>   - **다중 시점**: 사용자가 직접 조작하는 1인칭(FPS) 카메라와, 사전에 정의된 대피 경로를 따라 자동으로 움직이는 다중 시점을 제공합니다.
>   - **실감 효과**: 화면 흔들림과 같은 지진 효과와 함께, 대피 경로를 안내하는 화살표 등 시각적 UI를 구현했습니다.

-----

## ✨ 주요 기능

  - **1인칭 카메라**: 마우스로 시점을, WASD 키로 이동을 조작하며 점프와 앉기 기능도 지원합니다.
  - **지진 효과**: `shakeIntensity` 파라미터를 통해 조절 가능한 화면 흔들림 효과로 지진 상황을 연출합니다.
  - **자동 대피 경로**: `data/*.txt` 파일에 정의된 Waypoint를 파싱하여, 여러 가상 캐릭터가 지정된 경로를 따라 자동으로 대피하는 모습을 시뮬레이션합니다.
  - **안내 UI**: 대피 경로를 안내하는 화살표와 위험 구역을 표시하는 X 마크를 화면에 렌더링합니다.
  - **다양한 씬(Scene)**: 교실에서 시작하여 복도를 거쳐 운동장으로 이어지는 공간을 구성했습니다.

-----

## 🖥️ 실행 방법

### 1\. 빌드 (g++)

먼저 `bin` 디렉터리를 생성한 후, 플랫폼에 맞는 명령어로 컴파일합니다.

```bash
mkdir -p bin

# Linux / macOS
g++ src/main.cpp -o bin/earthquake -lglut -lGL -lGLU

# Windows (MSYS2/MinGW)
g++ src/main.cpp -o bin/earthquake.exe -lfreeglut -lopengl32 -lglu32
```

### 2\. 실행

```bash
./bin/earthquake
```

-----

## ⌨️ 조작법

| 입력 | 기능 |
| :--- | :--- |
| **마우스 이동** | 카메라 시점 회전 |
| **W, A, S, D** | 앞/왼쪽/뒤/오른쪽으로 이동 |
| **Spacebar** | 점프 |
| **Ctrl 또는 C** | 앉기 |
| **E** | 자동 대피 경로 시작 / 중지 |
| **1, 2, 3** | `camera_path_N.txt` 에 해당하는 경로 선택 |
| **Q** | 프로그램 종료 |

-----

## ⚙️ 경로 파일 구성 (`data/*.txt`)

  - 각 줄에 Waypoint의 `x y z` 좌표를 공백으로 구분하여 기록합니다.

#### 예시: `data/camera_path_1.txt`

```
3.2 0.0 -6.5
5.8 0.0 -6.5
9.0 0.0 -4.0
12.0 0.0  0.0
```

-----

## 🧠 구현 메모

  - **업데이트 루프**: `deltaTime` 기반의 프레임 독립적인 이동 로직을 구현하여 일정한 속도를 유지합니다.
  - **카메라**: `yaw`/`pitch` 값을 누적하여 시점을 계산하고, 마우스 감도(`sensitivity`)를 적용했습니다.
  - **경로 추종**: 현재 Waypoint에 도달하면 다음 지점으로 타겟을 전환하며, `look-at` 벡터를 목표 방향으로 부드럽게 보간(lerp/slerp)하여 자연스러운 시선 이동을 구현했습니다.

-----

## 🖼️ 스크린샷 / 데모

| 교실 | 복도 | 운동장 |
| :---: | :---: | :---: |
| \<img src="assets/cg-project-earthquake-scene-01-classroom.jpg" width="250"/\> | \<img src="assets/cg-project-earthquake-scene-02-hallway.png" width="250"/\> | \<img src="assets/cg-project-earthquake-scene-03-schoolyard.png" width="250"/\> |

**전체 시연 GIF:**

\<img src="assets/earthquake-simulation-demo.gif" alt="Demonstration GIF"/\>

-----

## 🗺️ 로드맵 (향후 개발 계획)

  - [ ] **경로 편집기**: UI를 통해 실시간으로 대피 경로를 편집하고 저장하는 기능
  - [ ] **자동 경로 생성**: 충돌 박스 또는 네비게이션 메시(NavMesh) 기반의 자동 경로 탐색
  - [ ] **효과 추가**: 사이렌, 진동 등 사운드 효과와 먼지 등 파티클 효과 추가
  - [ ] **그래픽 개선**: 조명, 그림자, 재질(Texture) 개선 및 성능 최적화

-----

## 🪪 라이선스

이 프로젝트는 [MIT 라이선스](https://opensource.org/licenses/MIT)를 따릅니다.
