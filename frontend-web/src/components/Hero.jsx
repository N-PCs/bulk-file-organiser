import { useState, useEffect } from 'react'
import './Hero.css'
import {
  IconWindows,
  IconFolder,
  IconFile,
  IconShield,
  IconSort,
  IconReport,
  IconImage,
  IconDocuments,
  IconAudio,
  IconVideo,
} from './Icons'

const looseFiles = [
  { name: 'photo.jpg', Icon: IconImage },
  { name: 'report.pdf', Icon: IconDocuments },
  { name: 'track.mp3', Icon: IconAudio },
  { name: 'movie.mp4', Icon: IconVideo },
  { name: 'backup.zip', Icon: IconFile },
]

const sortedFolders = [
  { name: 'Images/', count: 1, Icon: IconImage },
  { name: 'Documents/', count: 1, Icon: IconDocuments },
  { name: 'Audio/', count: 1, Icon: IconAudio },
  { name: 'Video/', count: 1, Icon: IconVideo },
  { name: 'Archives/', count: 1, Icon: IconFile },
]

const workflowNodes = [
  {
    id: 'folder',
    title: 'Pick folder',
    subtitle: 'Downloads/',
    Icon: IconFolder,
    x: 2,
    y: 6,
    w: 26,
    h: 36,
    accent: '#6366f1',
    type: 'trigger',
    preview: 'before',
  },
  {
    id: 'scan',
    title: 'Scan files',
    subtitle: '5 loose items',
    Icon: IconFile,
    x: 32,
    y: 17,
    w: 16,
    h: 16,
    accent: '#8b5cf6',
    type: 'action',
  },
  {
    id: 'classify',
    title: 'Classify',
    subtitle: 'By extension',
    Icon: IconSort,
    x: 52,
    y: 15,
    w: 36,
    h: 16,
    accent: '#a855f7',
    type: 'action',
  },
  {
    id: 'dryrun',
    title: 'Dry-run',
    subtitle: 'Preview moves',
    Icon: IconShield,
    x: 32,
    y: 58,
    w: 16,
    h: 14,
    accent: '#059669',
    type: 'action',
  },
  {
    id: 'organize',
    title: 'Organize',
    subtitle: 'Move to folders',
    Icon: IconFolder,
    x: 52,
    y: 50,
    w: 26,
    h: 36,
    accent: '#2563eb',
    type: 'action',
    preview: 'after',
  },
  {
    id: 'report',
    title: 'PDF report',
    subtitle: 'Save summary',
    Icon: IconReport,
    x: 82,
    y: 58,
    w: 16,
    h: 14,
    accent: '#dc2626',
    type: 'output',
  },
]

const workflowEdges = [
  { from: 'folder', fromSide: 'right', to: 'scan', toSide: 'left' },
  { from: 'scan', fromSide: 'right', to: 'classify', toSide: 'left' },
  { from: 'classify', fromSide: 'bottom', to: 'dryrun', toSide: 'top' },
  { from: 'dryrun', fromSide: 'right', to: 'organize', toSide: 'left' },
  { from: 'organize', fromSide: 'right', to: 'report', toSide: 'left' },
]

const categoryChips = [
  { label: 'Images', ext: '.jpg', Icon: IconImage, color: '#ec4899' },
  { label: 'Documents', ext: '.pdf', Icon: IconDocuments, color: '#f59e0b' },
  { label: 'Audio', ext: '.mp3', Icon: IconAudio, color: '#14b8a6' },
  { label: 'Video', ext: '.mp4', Icon: IconVideo, color: '#3b82f6' },
]

const CANVAS_H = 92

function getPort(node, side) {
  switch (side) {
    case 'right':
      return { x: node.x + node.w, y: node.y + node.h / 2 }
    case 'left':
      return { x: node.x, y: node.y + node.h / 2 }
    case 'bottom':
      return { x: node.x + node.w / 2, y: node.y + node.h }
    case 'top':
      return { x: node.x + node.w / 2, y: node.y }
    default:
      return { x: node.x + node.w / 2, y: node.y + node.h / 2 }
  }
}

function edgePath(fromNode, fromSide, toNode, toSide) {
  const a = getPort(fromNode, fromSide)
  const b = getPort(toNode, toSide)

  if (fromSide === 'right' && toSide === 'left') {
    const offset = Math.max(Math.abs(b.x - a.x) * 0.45, 3)
    return `M ${a.x} ${a.y} C ${a.x + offset} ${a.y}, ${b.x - offset} ${b.y}, ${b.x} ${b.y}`
  }

  if (fromSide === 'bottom' && toSide === 'top') {
    const offset = Math.max(Math.abs(b.y - a.y) * 0.45, 3)
    return `M ${a.x} ${a.y} C ${a.x} ${a.y + offset}, ${b.x} ${b.y - offset}, ${b.x} ${b.y}`
  }

  if (fromSide === 'right' && toSide === 'top') {
    const offsetX = Math.max(Math.abs(b.x - a.x) * 0.35, 3)
    const offsetY = Math.max(Math.abs(b.y - a.y) * 0.35, 3)
    return `M ${a.x} ${a.y} C ${a.x + offsetX} ${a.y}, ${b.x} ${b.y - offsetY}, ${b.x} ${b.y}`
  }

  const mx = (a.x + b.x) / 2
  const my = (a.y + b.y) / 2
  return `M ${a.x} ${a.y} Q ${mx} ${my}, ${b.x} ${b.y}`
}

function FolderPreview({ type, activeStep }) {
  if (type === 'before') {
    return (
      <div className="workflow-folder-preview workflow-folder-before">
        <div className="workflow-preview-badge workflow-preview-badge-before">Before</div>
        <div className="workflow-folder-path">
          <IconFolder />
          <span>Downloads/</span>
        </div>
        <ul className="workflow-mini-list">
          {looseFiles.map((file) => {
            const FileIcon = file.Icon
            const dimmed = activeStep > 0
            return (
              <li key={file.name} className={dimmed ? 'workflow-mini-dimmed' : ''}>
                <FileIcon />
                <span>{file.name}</span>
              </li>
            )
          })}
        </ul>
      </div>
    )
  }

  return (
    <div className="workflow-folder-preview workflow-folder-after">
      <div className="workflow-preview-badge workflow-preview-badge-after">After</div>
      <div className="workflow-folder-path">
        <IconFolder />
        <span>Downloads/</span>
      </div>
      <ul className="workflow-mini-list">
        {sortedFolders.map((folder) => {
          const FolderIcon = folder.Icon
          const lit = activeStep >= 4
          return (
            <li key={folder.name} className={lit ? 'workflow-mini-lit' : ''}>
              <FolderIcon />
              <span>{folder.name}</span>
              <span className="workflow-mini-count">{lit ? folder.count : '—'}</span>
            </li>
          )
        })}
      </ul>
    </div>
  )
}

export default function Hero() {
  const [activeStep, setActiveStep] = useState(0)
  const totalSteps = workflowNodes.length

  useEffect(() => {
    const timer = setInterval(() => {
      setActiveStep((s) => (s + 1) % totalSteps)
    }, 2000)
    return () => clearInterval(timer)
  }, [totalSteps])

  const activeNodeId = workflowNodes[activeStep]?.id
  const activeEdgeIndex = Math.max(0, activeStep - 1)

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

        <div className="hero-workflow card" aria-hidden="true">
          <div className="workflow-toolbar">
            <div className="workflow-toolbar-left">
              <span className="workflow-dot" />
              <span className="workflow-dot" />
              <span className="workflow-dot" />
              <span className="workflow-title">Organize workflow</span>
            </div>
            <span className="workflow-status">
              Step {activeStep + 1}/{totalSteps} · {workflowNodes[activeStep]?.title}
            </span>
          </div>

          <div className="workflow-canvas">
            <svg
              className="workflow-edges"
              viewBox={`0 0 100 ${CANVAS_H}`}
              preserveAspectRatio="none"
            >
              <defs>
                <linearGradient id="edge-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
                  <stop offset="0%" stopColor="#6366f1" />
                  <stop offset="100%" stopColor="#2563eb" />
                </linearGradient>
                <marker
                  id="edge-arrow"
                  markerWidth="4"
                  markerHeight="4"
                  refX="3.5"
                  refY="2"
                  orient="auto"
                  markerUnits="strokeWidth"
                >
                  <path d="M0,0 L4,2 L0,4 Z" fill="context-stroke" />
                </marker>
              </defs>
              {workflowEdges.map((edge, i) => {
                const from = workflowNodes.find((n) => n.id === edge.from)
                const to = workflowNodes.find((n) => n.id === edge.to)
                if (!from || !to) return null
                const isActive = i <= activeEdgeIndex
                const isCurrent = i === activeEdgeIndex
                return (
                  <path
                    key={`${edge.from}-${edge.to}`}
                    d={edgePath(from, edge.fromSide, to, edge.toSide)}
                    className={`workflow-edge ${isActive ? 'workflow-edge-done' : ''} ${isCurrent ? 'workflow-edge-active' : ''}`}
                    fill="none"
                    strokeWidth="0.35"
                    vectorEffect="non-scaling-stroke"
                    markerEnd="url(#edge-arrow)"
                  />
                )
              })}
            </svg>

            {workflowNodes.map((node, index) => {
              const Icon = node.Icon
              const isActive = node.id === activeNodeId
              const isDone = index < activeStep
              const isExpanded = Boolean(node.preview)

              return (
                <div
                  key={node.id}
                  className={`workflow-node workflow-node-${node.type} ${isExpanded ? 'workflow-node-expanded' : ''} ${isActive ? 'workflow-node-active' : ''} ${isDone ? 'workflow-node-done' : ''}`}
                  style={{
                    left: `${node.x}%`,
                    top: `${node.y}%`,
                    width: `${node.w}%`,
                    height: `${node.h}%`,
                  }}
                >
                  <span className="workflow-port workflow-port-in" />
                  <div className="workflow-node-body">
                    <div className="workflow-node-header">
                      <div className="workflow-node-icon" style={{ '--node-accent': node.accent }}>
                        <Icon />
                      </div>
                      <div className="workflow-node-text">
                        <span className="workflow-node-title">{node.title}</span>
                        <span className="workflow-node-sub">{node.subtitle}</span>
                      </div>
                    </div>

                    {node.preview && <FolderPreview type={node.preview} activeStep={activeStep} />}

                    {node.id === 'classify' && (
                      <div className="workflow-inline-chips">
                        {categoryChips.map((chip) => {
                          const ChipIcon = chip.Icon
                          const lit = activeStep >= 2
                          return (
                            <span
                              key={chip.label}
                              className={`workflow-chip ${lit ? 'workflow-chip-lit' : ''}`}
                              style={{ '--chip-color': chip.color }}
                            >
                              <ChipIcon />
                              {chip.ext}
                            </span>
                          )
                        })}
                      </div>
                    )}
                  </div>
                  <span className="workflow-port workflow-port-out" />
                  {isActive && <span className="workflow-pulse" />}
                </div>
              )
            })}
          </div>

          <div className="workflow-footer">
            <div className="workflow-progress">
              <div
                className="workflow-progress-fill"
                style={{ width: `${((activeStep + 1) / totalSteps) * 100}%` }}
              />
            </div>
            <span className="workflow-footer-text">
              {activeStep === 0 && 'Messy Downloads/ with 5 loose files'}
              {activeStep === 1 && 'Scanning file extensions…'}
              {activeStep === 2 && 'Matching .jpg .pdf .mp3 .mp4 to categories'}
              {activeStep === 3 && 'Dry-run preview — no files moved yet'}
              {activeStep === 4 && 'Sorted into Images/, Documents/, Audio/, Video/, Archives/'}
              {activeStep === 5 && 'PDF report exported — pipeline complete'}
            </span>
          </div>
        </div>
      </div>
    </section>
  )
}
