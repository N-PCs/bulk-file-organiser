import './Download.css'
import { IconWindows, IconCheck } from './Icons'

const PACKAGE_NAME = 'urFiles-windows.zip'
const PACKAGE_VERSION = '1.0.0'

const packageContents = [
  'organizer.exe — Native Windows application',
  'config.json — Customizable sorting rules',
  'run.bat — Launch the GUI instantly',
  'organizer.bat — CLI wrapper for scripting',
  'README.txt — Quick start guide',
]

export default function Download() {
  const handleDownload = () => {
    const link = document.createElement('a')
    link.href = `/${PACKAGE_NAME}`
    link.download = PACKAGE_NAME
    document.body.appendChild(link)
    link.click()
    document.body.removeChild(link)
  }

  return (
    <section id="download" className="section download">
      <div className="container">
        <div className="download-card card">
          <div className="download-content">
            <span className="section-label">Download</span>
            <h2 className="section-title">Get urFiles for Windows</h2>
            <p className="download-desc">
              Portable package — no installer, no admin rights required. Extract anywhere
              and double-click to start organizing. Your files never leave your machine.
            </p>

            <ul className="package-list">
              {packageContents.map((item) => (
                <li key={item}>
                  <IconCheck className="check-icon" />
                  {item}
                </li>
              ))}
            </ul>

            <button type="button" className="btn btn-primary btn-lg download-btn" onClick={handleDownload}>
              <IconWindows className="btn-icon" />
              Download urFiles v{PACKAGE_VERSION}
            </button>
            <p className="download-note">Portable ZIP · Windows 10 or later · x64</p>
          </div>

          <div className="download-side">
            <div className="sys-req card">
              <h4>System Requirements</h4>
              <dl>
                <div>
                  <dt>OS</dt>
                  <dd>Windows 10 / 11</dd>
                </div>
                <div>
                  <dt>Architecture</dt>
                  <dd>64-bit (x64)</dd>
                </div>
                <div>
                  <dt>Dependencies</dt>
                  <dd>None</dd>
                </div>
                <div>
                  <dt>Install type</dt>
                  <dd>Portable / extract and run</dd>
                </div>
              </dl>
            </div>

            <div className="cli-preview card">
              <h4>Command line</h4>
              <pre><code>{`.\run.bat

.\organizer.bat C:\\Downloads --dry-run`}</code></pre>
            </div>
          </div>
        </div>
      </div>
    </section>
  )
}
