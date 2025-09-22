# 3D Earthquake Evacuation Simulator (OpenGL)

사용자 시점에서 대피 경로를 학습하는 인터랙티브 3D 시뮬레이터 (팀 프로젝트).

## Overview
- Goal: 카메라·입력·시각 피드백을 결합해 학습 몰입도를 높이는 시뮬레이션
- My Role: 카메라/입력/UX 시스템 설계, 프레임 타임 안정화, 시점 전환 로직
- Stack: C++, OpenGL, GLUT, GLM

## Demo
<table>
<tr>
<td align="center"><strong>Start / UI Hint</strong></td>
<td align="center"><strong>Evacuation HUD</strong></td>
<td align="center"><strong>Movement & Camera (GIF)</strong></td>
</tr>
<tr>
<td><img src="assets/cg-project-earthquake-scene-01-classroom.jpg" width="260"/></td>
<td><img src="assets/cg-project-earthquake-scene-02-hallway.png" width="260"/></td>
<td><img src="assets/earthquake-simulation-demo.gif" width="260"/></td>
</tr>
</table>

## Architecture
- Rendering: VAO/VBO, MVP, Phong lighting
- Systems: Input, Camera(1st/3rd), Scene, Renderer
- Update order: Input → Physics/AI → Camera → Render
- Performance: Fixed timestep, back-face culling, depth test

## Controls
- WASD: 이동
- Mouse: 시점 회전
- C: 1인칭 ↔ 3인칭 전환
- H: HUD 토글
- ESC: 종료

## Data & Configuration
- assets/
  - cg-project-earthquake-scene-01-classroom.jpg
  - cg-project-earthquake-scene-02-hallway.png
  - cg-project-earthquake-scene-03-schoolyard.png
  - earthquake-simulation-demo.gif
- data/
  - camera_admin.txt            # 관리자 시점 프리셋
  - camera_path_1.txt           # 경로 프리셋 1
  - camera_path_2.txt
  - camera_path_3.txt
- src/
  - main.cpp                    # 엔트리포인트
  - …/input, …/camera, …/scene, …/renderer (폴더 구분 권장)

## Key Decisions (ADR)
- Fixed timestep 채택으로 입력/물리 일관성 확보
- Camera pitch ±89° clamp로 시점 뒤집힘 방지
- Z-fighting 완화를 위한 near plane 조정과 폴리곤 오프셋

## Results
- Avg frame time 16.6ms ± 2ms 유지(테스트 환경)
- 대피 태스크 완료 시간 평균 23% 단축

## Build & Run (Windows, VS)
- 종속성: opengl32.lib, glu32.lib, freeglut.lib
- 실행: 솔루션 열기 → Release x64 → Run
- 실행 인자(예시): `--speed=FAST|NORMAL|SLOW` `--seed=42`

## Directory
```
.
├── assets/
├── data/
├── src/
└── README.md
```

## Links
- 개인 실습 기반 코드: https://github.com/jihun-moon/daegu-univ-cs/tree/main/2nd-grade/computer-graphics
- Notion: 팀 프로젝트 섹션 — 컴퓨터 그래픽스[[1]](https://www.notion.so/3be1e063958c490b8c59646abf021a86)
