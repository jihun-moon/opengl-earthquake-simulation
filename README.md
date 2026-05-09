<h1 align="center">🏫 OpenGL Earthquake Evacuation Simulation</h1>

<p align="center"><em>An interactive 3D earthquake evacuation training simulator built from scratch in C++ &amp; OpenGL.</em></p>

<p align="center">
  <img src="https://img.shields.io/badge/license-MIT-blue?style=flat-square"/>
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=cplusplus&logoColor=white"/>
  <img src="https://img.shields.io/badge/OpenGL-Fixed_Pipeline-5586A4?style=flat-square&logo=opengl&logoColor=white"/>
  <img src="https://img.shields.io/badge/GLUT-FreeGLUT-orange?style=flat-square"/>
  <img src="https://img.shields.io/badge/Domain-Safety_Education-success?style=flat-square"/>
  <a href="https://www.notion.so/3D-e7aa1e73062e477ba05e0971de5bef18?source=copy_link"><img src="https://img.shields.io/badge/Deep_Dive-Notion-000000?style=flat-square&logo=notion&logoColor=white"/></a>
</p>

<p align="center">
  <img src="assets/earthquake-simulation-demo.gif" alt="Demo" width="60%">
</p>

---

## 🎯 Why this project

학교/공공시설의 지진 대피 훈련은 한 번뿐인 단방향 강의로 끝나는 경우가 많습니다. 이 프로젝트는 **학생이 직접 1인칭 시점으로 교실 → 복도 → 운동장 대피 동선을 체험** 하도록 만들어, 안전 교육의 몰입도와 기억 정착률을 높이는 것을 목표로 합니다.

> Computer Graphics 학기 프로젝트 — 평가 점수 외에도 "현장에서 실제로 쓸 만한 도구" 를 만들 수 있는지 도전한 사례.

---

## ✨ Key Features

- 🎥 **인터랙티브 1인칭 카메라** — 마우스/키보드로 자유로운 학습자 시점
- 🎬 **시네마틱 가이드 뷰** — 강사가 정해진 경로 (`data/camera_path_*.txt`) 로 자동 시연
- 🏛 **3가지 시나리오** — 교실 → 복도 → 운동장 (다단계 대피)
- 🌀 **지진 흔들림 효과** — 시점 셰이크 + 오브젝트 진동
- 📜 **데이터 기반 카메라 경로** — 코드 수정 없이 텍스트 파일로 시연 동선 변경 가능

---

## 🏗 Architecture

```
┌─────────────────────────────────────────────────┐
│              main loop (GLUT)                   │
│   ┌──────────────┐    ┌──────────────────────┐  │
│   │ Input handler │───▶│  Camera (free / path)│  │
│   └──────────────┘    └──────────┬───────────┘  │
│                                  │               │
│   ┌────────────────┐    ┌────────▼───────────┐  │
│   │ Scene loader   │───▶│  Render pipeline   │  │
│   │ (classroom /   │    │  (lighting, shake, │  │
│   │  hallway / yard│    │   primitives)      │  │
│   └────────────────┘    └────────────────────┘  │
└─────────────────────────────────────────────────┘
                  ▲
                  │ camera_path_*.txt
        ┌─────────┴──────────┐
        │  Director scripts  │
        └────────────────────┘
```

---

## 🛠 Tech Stack

| Layer | Tool | Role |
|---|---|---|
| Language | **C++17** | core simulation |
| Graphics | **OpenGL (fixed-function)** | rasterization, lighting |
| Windowing | **GLUT / FreeGLUT** | OS event loop, input |
| Data | Plain text path files | director / cinematic mode |

---

## 📦 Build & Run

**Linux (g++ + FreeGLUT):**

```bash
sudo apt-get install freeglut3-dev
g++ src/main.cpp -o simulation -lGL -lGLU -lglut
./simulation
```

**macOS (clang):**

```bash
clang++ src/main.cpp -o simulation -framework OpenGL -framework GLUT
./simulation
```

**Windows:** Visual Studio + FreeGLUT 또는 NuGet `nupengl.core` 패키지 권장.

---

## 📂 Project Layout

```
.
├── src/
│   └── main.cpp                # core simulation
├── data/
│   ├── camera_admin.txt        # admin (free) view config
│   ├── camera_path_1.txt       # cinematic path: classroom escape
│   ├── camera_path_2.txt       # cinematic path: hallway
│   └── camera_path_3.txt       # cinematic path: schoolyard
├── assets/                     # demo GIF, screenshots
└── README.md
```

---

## 🎬 Screenshots

| 교실 | 복도 | 운동장 |
|:---:|:---:|:---:|
| <img src="assets/cg-project-earthquake-scene-01-classroom.jpg" width="100%"> | <img src="assets/cg-project-earthquake-scene-02-hallway.png" width="100%"> | <img src="assets/cg-project-earthquake-scene-03-schoolyard.png" width="100%"> |

---

## 📚 Deep Dive

설계 의도, 시도한 접근, 막혔던 지점, 해결 과정은 Notion 에 정리되어 있습니다 — 코드만 보면 안 보이는 부분이 더 중요합니다.

➡ [**OpenGL 지진 대피 시뮬레이션 — 프로젝트 회고**](https://www.notion.so/3D-e7aa1e73062e477ba05e0971de5bef18?source=copy_link)

---

## 📄 License

MIT — see [LICENSE](LICENSE).

---

<p align="center">
  Built by <a href="https://github.com/jihun-moon">@jihun-moon</a> · <a href="mailto:jihun0948@naver.com">jihun0948@naver.com</a>
</p>
