# Sticky Note (Qt6)

A lightweight, feature-rich sticky note application for Linux (Ubuntu) built with C++23 and the Qt6 framework.

![Sticky Note Icon](icons/note_icon.png)

## Features

- **Create & Edit Notes:** Easily create new notes and edit their content with a clean interface.
- **Markdown Support:** Format your notes using simple Markdown syntax.
- **Customizable Colors:** Choose from a set of predefined pastel colors or select any custom color for each note.
- **Pin to Top:** Pin important notes to keep them above all other windows.
- **Persistence:** Your notes are automatically saved and restored when you restart the application.
- **System Tray Integration:** Access all features from the system tray, including 'Restore', 'Close All Notes', and '
  Quit'.
- **Borderless & Resizable:** Frameless windows that can be moved and resized with ease.

## Requirements

- **OS:** Linux (Tested on Ubuntu)
- **C++ Compiler:** C++23 support
- **Framework:** Qt6 (Widgets)
- **Build System:** CMake (>= 3.14)

## Installation & Build

### From Source

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd Sticky_Note_QT
   ```

2. **Configure and Build:**
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --target sticky_note
   ```

3. **Run the application:**
   ```bash
   ./build/sticky_note
   ```

### AppImage

You can also generate an AppImage for portability:

```bash
cmake --build build --target appimage
```

## Usage

- **System Tray:** Use the system tray icon to create new notes, restore the main window, or close all notes.
- **Note Actions:**
    - **Pen Icon:** Edit the note.
    - **Color Wheel:** Change the note's color.
    - **Pin:** Toggle 'Stay on Top'.
    - **Exit:** Close and delete the note.
- **Drag & Resize:** Drag the note from any area to move it, or use the edges to resize.
- **Keyboard Shortcuts:**
    - `Ctrl + N`: Create a new note.
    - `Ctrl + E`: Toggle edit mode.
    - `Ctrl + S`: Save note.
    - `Ctrl + C`: Change note color.
    - `Ctrl + P`: Toggle pin (Stay on Top).
    - `Ctrl + W`: Close the current note.

## License

*Check the project for licensing information.*