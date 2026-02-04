# Build Instructions

## Requirements

- Visual Studio 2019/2022
- DirectX SDK
- Windows SDK

## Quick Build

```
Open DayZ Steam.sln
Select Release x64
Build (F7)
```

Output: `x64\Release\Steam.exe`

## Project Files

```
DayZ-External/
├── main.cpp
├── memory_optimized.h/cpp
├── overlay/
│   ├── overlay.cpp
│   ├── globals.h/cpp
│   └── drawings_optimized.h
├── hck/
│   ├── entity.cpp
│   ├── ConfigSystem.h/cpp
│   ├── ModernMenu.h/cpp
│   └── MenuUI.cpp
└── Imgui/
```

## Usage

1. Run DayZ
2. Run Steam.exe as admin
3. Press INSERT for menu

Configs saved in `x64/Release/configs/`

## Troubleshooting

Build errors:
- Install DirectX SDK
- Check all .cpp files in project
- Build x64 Release

Runtime:
- Run as admin
- Check antivirus

Low FPS:
- Reduce ESP distances
- Build Release not Debug
