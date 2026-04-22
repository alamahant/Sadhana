# Changelog

All notable changes to Sadhana will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

---

## [1.0.1] - 2026-04-24

### Changed
- **Temporarily disabled default deity modules** (Chenrezig, Tara, Guru, Manjushri) to make the application tradition-agnostic out of the box.
- Default modules may be restored in a future update based on user feedback.

### Added
- Instructional example module (`example.json`) that teaches users how to use Sadhana without assuming any specific religious tradition.
- Clear emphasis on custom module creation, encouraging users to build practices that fit their own needs.

### Project Direction
Sadhana now launches as a neutral practice tracking tool. Users are guided to create their own multi-stage modules rather than relying on pre-installed deity content.

---

## [1.0.0] - 2026-04-14

### Added
- Initial release of Sadhana.

### Core Features
- **Module System** — Default modules for Chenrezig, Tara, Guru, and Manjushri.
- **Counter System** — Track lifetime repetitions per module. Add session counts. Reset counter.
- **Journal** — Record every practice session with date, module name, and repetition count. Browse entries by calendar date.
- **Audio Support** — Assign MP3, WAV, FLAC, M4A, OGG, WEBM, AAC, WMA, OPUS files to any stage. Play, pause, stop, repeat, volume control, progress slider.
- **Image Support** — Assign images to modules. Fullscreen zoomable view with I key. Mouse wheel zoom, drag to pan.
- **PDF Support** — Attach PDF documents to stages. Multi-page view. Zoom in/out (Ctrl+ / Ctrl-). Fit to width (Ctrl+9). Fit to page (Ctrl+8). Reset zoom (Ctrl+0).
- **Custom Modules** — Create custom modules with unlimited stages. Edit liturgy text, mantra text, stage names in real time. Assign audio, images, PDFs per stage. Delete modules (right-click on grid).
- **Tibetan Calendar** — Special observance days (Düchen) highlighted. Karmic multipliers shown. Tibetan lunar dates displayed.
- **Reader Tool** — Open text and PDF files. Edit and save text documents. Multiple tabs. Open with Ctrl+R.

### Keyboard Shortcuts
- `Ctrl+H` — Return to home grid
- `Ctrl+R` — Open Reader
- `Ctrl+K` — Open Tibetan Calendar
- `Ctrl+J` — Open Journal
- `F` — Toggle fullscreen (text/PDF)
- `I` — Fullscreen image view
- `Esc` — Exit fullscreen
- `Ctrl+` / `Ctrl-` — Zoom in/out (PDF or text)
- `Ctrl+0` — Reset zoom to 100%
- `Ctrl+9` — PDF: Fit to width
- `Ctrl+8` — PDF: Fit to page

### Data Storage
- Standard install: `Documents/Sadhana/`
- Flatpak install: `~/.var/app/io.github.alamahant.Sadhana/data/Sadhana/`
- Custom modules saved as JSON files
- Journal entries saved as JSON database

---
