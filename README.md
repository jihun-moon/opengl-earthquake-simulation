# OpenGL 지진 대피 시뮬레이션

C++와 OpenGL을 사용하여 구현한 3D 지진 대피 시뮬레이션 프로젝트입니다. 상세 문서, 스크린샷, 회고는 Notion에 정리되어 있습니다.

## 🔗 Notion

  - **상세 문서:** [OpenGL 지진 대피 시뮬레이션 프로젝트 상세 문서](https://www.notion.so/3D-e7aa1e73062e477ba05e0971de5bef18?source=copy_link)

## 🚀 실행

이 프로젝트는 C++와 OpenGL/GLUT 라이브러리를 사용하여 개발되었습니다. 실행하려면 C++ 컴파일러와 GLUT 라이브러리가 필요합니다.

**필요 환경:**

  - C++ 컴파일러 (g++, Clang, MSVC 등)
  - GLUT (OpenGL Utility Toolkit) 라이브러리

**빌드 및 실행 예시 (g++ 사용):**

```bash
# GLUT 라이브러리가 설치되어 있어야 합니다.
g++ main.cpp -o simulation -lGL -lGLU -lglut

# 컴파일 후 실행 파일을 실행합니다.
./simulation
```

## 📂 폴더 구조

프로젝트 파일들을 다음과 같이 구성하는 것을 추천합니다.

  - `src/`: C++ 소스 코드 (`main.cpp`) 가 위치합니다.
  - `data/`: 캐릭터 이동 경로 데이터 (`camera_path_1.txt` 등) 가 위치합니다.
  - `docs/`: 프로젝트 관련 문서 (발표 자료, 스크린샷 등) 를 저장합니다.

## 🖼️ 스크린샷

<img src="assets/earthquake-simulation-demo.gif" alt="데모" width="48%">

## 📄 라이선스

이 프로젝트는 **MIT 라이선스**를 따릅니다.

  - [MIT License](https://opensource.org/licenses/MIT)
