# Earthquake Simulation 3D

지진 상황별 대피 행동 학습용 실시간 3D 시뮬레이터. OpenGL·C++ 기반 렌더링 파이프라인 최적화로 안정적 프레임 타임을 확보하고, 입력 지연과 시점 전환 멀미를 줄였습니다.

- Engine/Lib: OpenGL, GLUT, GLM
- Language: C++
- Topics: Rendering pipeline, Performance profiling, Input-Camera UX

## 🎥 주요 기능 시연

| 지진 발생 시뮬레이션 (교실) | 대피 경로 안내 (복도 → 운동장) |
| :---: | :---: |
| <img src="assets/quake-simulation-demo.gif" alt="지진 발생 시뮬레이션" width="400"/> | <img src="assets/evacuation-path-run.gif" alt="대피 경로 안내" width="400"/> |
| 1. 지진이 발생하면, 안내에 따라 책상 밑으로 신속히 대피합니다. | 2. 진동이 멈춘 후, 머리를 보호하며 질서있게 운동장으로 대피합니다. |

## Features
- 시나리오 기반 대피 학습 플로우와 HUD 피드백(화살표·거리)
- VAO/VBO 재사용, 상태 배치로 안정적 프레임 타임
- 계측·로깅으로 리그레션 탐지

## Architecture
- Loop: Input → Update(fixed Δt) → Effects(HUD/Shake) → Render
- Rendering: VAO/VBO, 셰이더, MVP 변환, 깊이 테스트/컬링
- Input/Camera: 폴링 기반 입력, 1인칭·3인칭 전환, Pitch 클램프, 곡선 보간

## Before/After (Problems → Solutions → Results)
- 프레임 타임 스파이크 → VAO/VBO 재사용·상태 배치 → 16.6ms ± 2ms 유지
- 입력 누락/지연 → 폴링 + 고정 Δt 루프 → 테스트 상 누락 0건
- 시점 전환 멀미 → Pitch 클램프·감속 곡선 보간 → 사용자 불편 신고 감소

## Code Snippet
```cpp
// Fixed timestep update and simple camera clamp
const float dt = 1.f / 60.f;
float acc = 0.f;
while (running) {
  acc += GetFrameDelta();
  while (acc >= dt) {
    HandleInput(dt);             // client-side input
    UpdateSimulation(dt);        // deterministic update
    acc -= dt;
  }
  Render();                      // draw with current state
}

void Camera::ClampPitch() {
  pitch = std::clamp(pitch, -89.0f, 89.0f);
}
```

## Folder Structure
```
/Config
/Content or Assets
/Source               # C++ sources
/tools/benchmark      # optional micro-bench scripts
/assets               # README gifs: quake-simulation-demo.gif, evacuation-path-run.gif
README.md
```

## Build & Run
1. Prerequisites
   - C++17 toolchain, CMake ≥ 3.20
   - OpenGL, GLUT/GLFW, GLM
2. Configure & Build
   - cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   - cmake --build build --config Release
3. Run
   - ./build/bin/earthquake-sim

## Profiling
- Frame-time 로그 출력 옵션 제공
- RenderDoc 또는 GPUView로 드로우콜/파이프라인 분석
- 반복 벤치마크 스크립트로 리그레션 체크

## Roadmap
- [ ] PBR 라이팅 및 포스트 프로세싱
- [ ] 충돌/네비게이션 메쉬 기반 경로 안내
- [ ] 데이터 주도형 시나리오 시스템

## Timeline & Links
- 2024-10-28 ~ 2024-12-09
- Notion: Earthquake Simulation 3D[[1]](https://www.notion.so/8483686769e449338190cd1eb0fe5840)
- GitHub: 프로젝트 레포 링크 삽입

## License
MIT 또는 프로젝트 정책에 맞는 라이선스 표기
