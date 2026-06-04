import './Download.css'
import { IconWindows, IconCheck, IconDownload } from './Icons'

const VERSION = '1.0.0'

const platforms = [
  {
    id: 'windows',
    name: 'Windows',
    icon: IconWindows,
    zipName: 'urfm-windows.zip',
    description: 'Native Win32 GUI — no dependencies, no admin rights required.',
    contents: [
      'organizer.exe — Native Windows application',
      'config.json — Customizable sorting rules',
      'run.bat — Launch the GUI instantly',
      'organizer.bat — CLI wrapper for scripting',
      'README.txt — Quick start guide',
    ],
    note: 'Portable ZIP · Windows 10 or later · x64',
    sysReqs: { OS: 'Windows 10 / 11', Architecture: '64-bit (x64)', Dependencies: 'None', 'Install type': 'Portable' },
    cli: ['.\\run.bat', '.\\organizer.bat C:\\Downloads --dry-run'],
  },
  {
    id: 'linux',
    name: 'Linux',
    icon: IconDownload,
    zipName: 'urfm-linux.tar.gz',
    description: 'FLTK-based GUI for all major distributions. Build from source.',
    contents: [
      'urfm — Native Linux binary (FLTK GUI + CLI)',
      'config.json — Customizable sorting rules',
      'build.sh — Build script for Fedora, Ubuntu, Arch',
      'README.txt — Quick start guide',
    ],
    note: 'Source tarball · Fedora / Ubuntu / Arch · x64',
    sysReqs: { OS: 'Fedora / Ubuntu / Arch', Architecture: '64-bit (x64)', Dependencies: 'FLTK 1.3+', 'Install type': 'Build from source' },
    cli: ['sudo apt install libfltk1.3-dev   # Ubuntu', 'sudo dnf install fltk-devel           # Fedora', './build.sh && ./urfm ~/Downloads --dry-run'],
  },
  {
    id: 'mac',
    name: 'macOS',
    icon: IconDownload,
    zipName: 'urfm-macos.tar.gz',
    description: 'Native macOS experience via FLTK. Requires Homebrew for dependencies.',
    contents: [
      'urfm — Native macOS binary (FLTK GUI + CLI)',
      'config.json — Customizable sorting rules',
      'build_mac.sh — Build script for macOS',
      'README.txt — Quick start guide',
    ],
    note: 'Source tarball · macOS 11+ · x64 & Apple Silicon',
    sysReqs: { OS: 'macOS 11 (Big Sur)+', Architecture: 'x64 & Apple Silicon', Dependencies: 'FLTK (brew install fltk)', 'Install type': 'Build from source' },
    cli: ['brew install fltk', 'chmod +x build_mac.sh', './build_mac.sh && ./urfm ~/Downloads --dry-run'],
  },
]

export default function Download() {
  const handleDownload = (zipName) => {
    const link = document.createElement('a')
    link.href = `/${zipName}`
    link.download = zipName
    document.body.appendChild(link)
    link.click()
    document.body.removeChild(link)
  }

  return (
    <section id="download" className="section download">
      <div className="container">
        <div className="download-header">
          <span className="section-label">Download</span>
          <h2 className="section-title">Available for all platforms</h2>
          <p className="section-subtitle">
            Native C++ performance on Windows, Linux, and macOS. No runtime required.
          </p>
        </div>

        <div className="download-grid">
          {platforms.map((platform) => (
            <div key={platform.id} className="download-platform-card card">
              <div className="platform-top">
                <platform.icon className="platform-icon" />
                <h3 className="platform-name">{platform.name}</h3>
              </div>
              <p className="platform-desc">{platform.description}</p>

              <ul className="package-list">
                {platform.contents.map((item) => (
                  <li key={item}>
                    <IconCheck className="check-icon" />
                    {item}
                  </li>
                ))}
              </ul>

              <button
                type="button"
                className="btn btn-primary btn-lg download-btn"
                onClick={() => handleDownload(platform.zipName)}
              >
                <IconDownload className="btn-icon" />
                Download v{VERSION}
              </button>
              <p className="download-note">{platform.note}</p>

              <details className="platform-details">
                <summary>System requirements & CLI</summary>
                <div className="sys-req card">
                  <dl>
                    {Object.entries(platform.sysReqs).map(([k, v]) => (
                      <div key={k}>
                        <dt>{k}</dt>
                        <dd>{v}</dd>
                      </div>
                    ))}
                  </dl>
                </div>
                <div className="cli-preview card">
                  <h4>Command line</h4>
                  <pre><code>{platform.cli.join('\n')}</code></pre>
                </div>
              </details>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}
