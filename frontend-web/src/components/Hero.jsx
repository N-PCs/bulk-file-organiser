import { useState, useEffect, useCallback } from 'react'
import './Hero.css'
import { IconWindows, IconFolder, IconFile, IconDocuments, IconImage, IconAudio, IconVideo } from './Icons'

const looseFiles = [
  { name: 'photo.jpg', category: 'Images', icon: IconImage },
  { name: 'report.pdf', category: 'Documents', icon: IconDocuments },
  { name: 'track.mp3', category: 'Audio', icon: IconAudio },
  { name: 'movie.mp4', category: 'Video', icon: IconVideo },
  { name: 'backup.zip', category: 'Archives', icon: IconFile },
]

const sortedFolders = [
  { name: 'Images/', count: 2, icon: IconImage },
  { name: 'Documents/', count: 1, icon: IconDocuments },
  { name: 'Audio/', count: 1, icon: IconAudio },
  { name: 'Video/', count: 1, icon: IconVideo },
  { name: 'Archives/', count: 1, icon: IconFile },
]

const STATUS = {
  SCANNING: 'scanning',
  MOVING: 'moving',
  COMPLETE: 'complete',
}

export default function Hero() {
  const [step, setStep] = useState(0)
  const [movedFiles, setMovedFiles] = useState([])
  const [currentStatus, setCurrentStatus] = useState(STATUS.SCANNING)
  const [progress, setProgress] = useState(0)
  const [activeFile, setActiveFile] = useState(null)

  const totalFiles = looseFiles.length

  const reset = useCallback(() => {
    setStep(0)
    setMovedFiles([])
    setCurrentStatus(STATUS.SCANNING)
    setProgress(0)
    setActiveFile(null)
  }, [])

  useEffect(() => {
    if (step >= totalFiles) {
      const timer = setTimeout(() => {
        reset()
      }, 3000)
      return () => clearTimeout(timer)
    }

    const scanTimer = setTimeout(() => {
      setCurrentStatus(STATUS.MOVING)
      setActiveFile(step)

      const moveTimer = setTimeout(() => {
        setMovedFiles((prev) => [...prev, looseFiles[step].category])
        setProgress(((step + 1) / totalFiles) * 100)
        setActiveFile(null)
        setStep((s) => s + 1)
        setCurrentStatus(step + 1 >= totalFiles ? STATUS.COMPLETE : STATUS.SCANNING)
      }, 800)

      return () => clearTimeout(moveTimer)
    }, 400)

    return () => clearTimeout(scanTimer)
  }, [step, totalFiles, reset])

  const getFolderCount = (category) => {
    return movedFiles.filter((f) => f === category).length
  }

  return (
    <section className="hero">
      <div className="container hero-layout">
        <div className="hero-copy">
          <p className="hero-eyebrow">Cross-platform file organizer</p>
          <h1 className="hero-title">
            Turn a messy folder into{' '}
            <span className="hero-gradient">organized categories</span>
          </h1>
          <p className="hero-description">
            Point urFileManager at any cluttered directory — like Downloads — and it moves
            every loose file into subfolders by type. Images, Documents, Audio, Video,
            and Archives are created automatically. Preview with dry-run before anything moves.
          </p>

          <ul className="hero-points">
            <li>Only sorts files sitting directly in the folder — subfolders stay untouched</li>
            <li>Renames on conflict instead of overwriting existing files</li>
            <li>Exports a PDF report and keeps a full log of every action</li>
          </ul>

          <div className="hero-actions">
            <a href="#download" className="btn btn-primary btn-lg">
              <IconWindows className="btn-icon" />
              Download urFM
            </a>
            <a href="#download" className="btn btn-secondary btn-lg">
              CLI install commands
            </a>
          </div>
        </div>

        <div className="hero-demo card" aria-hidden="true">
          <div className="demo-header">
            <span className="demo-dot" />
            <span className="demo-dot" />
            <span className="demo-dot" />
            <span className="demo-title">urFileManager — Downloads</span>
          </div>

          <div className="demo-body">
            <div className={`demo-panel demo-before ${currentStatus === STATUS.SCANNING ? 'demo-panel-active' : ''}`}>
              <p className="demo-label">Before — loose files</p>
              <div className="demo-folder-row">
                <IconFolder />
                <span>Downloads/</span>
              </div>
              <ul className="demo-file-list">
                {looseFiles.map((file, i) => {
                  const Icon = file.icon
                  const isDone = movedFiles.includes(file.category) || i < step
                  const isActive = activeFile === i
                  return (
                    <li
                      key={file.name}
                      className={`${isDone ? 'demo-file-done' : ''} ${isActive ? 'demo-file-active' : ''}`}
                    >
                      <Icon />
                      <span className="demo-file-name">{file.name}</span>
                      {isDone && <span className="demo-file-check">✓</span>}
                    </li>
                  )
                })}
              </ul>
            </div>

            <div className="demo-flow">
              <div className="demo-flow-line" />
              <span className={`demo-flow-badge ${currentStatus}`}>
                {currentStatus === STATUS.SCANNING && 'Scanning...'}
                {currentStatus === STATUS.MOVING && `Move to ${looseFiles[activeFile]?.category}`}
                {currentStatus === STATUS.COMPLETE && 'Complete!'}
              </span>
              <svg viewBox="0 0 24 24" fill="none" className={`demo-arrow ${currentStatus === STATUS.COMPLETE ? 'demo-arrow-done' : ''}`}>
                <path d="M12 5v14M12 19l-5-5M12 19l5-5" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
              </svg>
            </div>

            <div className={`demo-panel demo-after ${currentStatus === STATUS.MOVING || currentStatus === STATUS.COMPLETE ? 'demo-panel-active' : ''}`}>
              <p className="demo-label">After — sorted folders</p>
              <ul className="demo-folder-list">
                {sortedFolders.map((folder) => {
                  const count = getFolderCount(folder.name.replace('/', ''))
                  return (
                    <li key={folder.name} className={count > 0 ? 'demo-folder-has' : ''}>
                      <folder.icon />
                      <span>{folder.name}</span>
                      <span className={`demo-count ${count > 0 ? 'demo-count-filled' : ''}`}>
                        {count > 0 ? count : 0}
                      </span>
                    </li>
                  )
                })}
              </ul>
            </div>
          </div>

          <div className="demo-footer">
            <div className="demo-progress">
              <div
                className="demo-progress-fill"
                style={{ width: `${progress}%` }}
              />
            </div>
            <span>
              {currentStatus === STATUS.COMPLETE
                ? `Organized ${totalFiles} files into ${totalFiles} categories`
                : `Organizing ${movedFiles.length} of ${totalFiles} files`}
            </span>
          </div>
        </div>
      </div>
    </section>
  )
}
