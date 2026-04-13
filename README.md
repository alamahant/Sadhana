# Sadhana

**A Digital Shrine and Practice Tracker for Buddhist Practitioners**

Sadhana is a cross-platform desktop application designed to support and enhance your daily Buddhist practice. It provides a structured, customizable environment for working with traditional liturgies, mantras, and deity visualizations, while helping you track your progress over time.

> **Note:** Although made with Tibetan Buddhism in mind and intent, Sadhana can be used for any puja or sadhana practice, including Hindu, Chinese, or Christian contemplative traditions. The custom module system allows you to create practice sequences for any spiritual path.

---

## ✨ Features

### 🧘 Deity Practice Modules

Pre-loaded with four traditional modules following a classical four-stage structure:

| Stage | Purpose |
|-------|---------|
| **About** | Deity introduction and iconography |
| **Opening** | Invocation and refuge prayers |
| **Main Practice** | Mantra recitation with counter |
| **Closing** | Dedication of merit |

**Included Deities:**
- Chenrezig (Avalokiteśvara) — Embodiment of Compassion
- Green Tara (Ārya Tārā) — Swift Liberatress
- Guru Rinpoche (Padmasambhava) — Precious Master of Oddiyana
- Mañjuśrī — Bodhisattva of Wisdom

### 🛠️ Custom Modules

Create your own practice modules with **unlimited stages**. Each stage supports:

| Feature | Description |
|---------|-------------|
| **Liturgy Text** | Write or load from `.txt` files |
| **Mantra** | Custom mantra text with display toggle |
| **Image** | Assign deity or practice images |
| **Audio** | Attach guided meditations or chants (MP3, WAV, FLAC, etc.) |
| **PDF** | Embed practice texts or sadhana manuals |
| **Counter** | Per-stage lifetime mantra accumulation |

### 📿 Practice Tracking

- **Lifetime Counter** — Persistent count across all sessions
- **Session Spinbox** — Record repetitions for the current session
- **One-Click Add** — Updates total and logs to journal automatically

### 📓 Integrated Journal

- Automatically logs each session with:
  - Date and time
  - Module and stage name
  - Repetition count
- Add personal notes and reflections
- Browse entries by calendar date

### 📅 Tibetan Buddhist Calendar

- Displays Tibetan lunar dates alongside Gregorian calendar
- Highlights special observance days (*Düchen*)
- Shows karmic multipliers (e.g., 100 million × on Buddha Days)
- Pre-loaded with data from 2025–2049

### 🎵 Audio Player

- Built-in media player with play/pause, stop, and progress seeking
- Volume control
- Optional repeat mode for continuous mantra playback

### 📖 Reader & Editor

- Multi-tab interface for text files, PDFs, and plain-text editing
- Zoom controls and fit-to-width/page for PDFs
- Save edited documents to disk

### 🖼️ Image Viewer

- Fullscreen zoomable image view (press `I` when image is visible)
- Smooth scaling and panning

---

## ⌨️ Keyboard Shortcuts

### Global

| Shortcut | Action |
|----------|--------|
| `Ctrl + H` | Return to Home Grid |
| `Ctrl + R` | Open Reader |
| `Ctrl + K` | Open Tibetan Calendar |
| `F` | Toggle fullscreen (text/PDF only) |
| `I` | Enter image fullscreen mode |
| `Esc` | Exit fullscreen |

### PDF Viewer

| Shortcut | Action |
|----------|--------|
| `Ctrl + +` | Zoom in |
| `Ctrl + -` | Zoom out |
| `Ctrl + 0` | Reset to 100% |
| `Ctrl + 9` | Fit to width |
| `Ctrl + 8` | Fit to page |

### Text Viewer

| Shortcut | Action |
|----------|--------|
| `Ctrl + +` | Increase font size |
| `Ctrl + -` | Decrease font size |
| `Ctrl + 0` | Reset font size |

---



## 🏗️ Technical Overview

| Component | Technology |
|-----------|------------|
| **Language** | C++17 |
| **Framework** | Qt 6 |
| **Audio** | QMediaPlayer + QAudioOutput |
| **PDF** | QPdfDocument + QPdfView |
| **Serialization** | JSON (QJsonDocument) |
| **Build System** | CMake / qmake |
| **Platforms** | Linux (Flatpak), Windows, macOS |

### Architecture Highlights

- **ModuleManager** — Singleton managing default/custom modules with JSON persistence
- **PujaView** — Main practice interface with dynamic stage loading
- **GridView** — Visual module selection with customizable images
- **JournalManager** — Session logging with calendar-based retrieval
- **TibetanCalendar** — Lunar date calculation and observance highlighting

---

## 🚀 Installation

### Flatpak (Linux)

```bash
flatpak install io.github.alamahant.Sadhana
```

### Build from Source

```bash
git clone https://github.com/alamahant/Sadhana.git
cd Sadhana
mkdir build && cd build
cmake ..
make
```
---

## 📝 License & Copyright

© 2026 Alamahant. All rights reserved.

---

## 🙏 Dedication

> *By this merit, may I swiftly attain the state of Chenrezig.*
> *And may every single living being without exception be established in that very same level.*

May this application serve as a support on the path to liberation.

---