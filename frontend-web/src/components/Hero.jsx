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

const workflowNodes = [
  {
    id: 'folder',
    title: 'Pick folder',
    subtitle: 'Downloads/',
    Icon: IconFolder,
    x: 6,
    y: 14,
    accent: '#6366f1',
    type: 'trigger',
  },
  {
    id: 'scan',
    title: 'Scan files',
    subtitle: '5 loose items',
    Icon: IconFile,
    x: 34,
    y: 14,
    accent: '#8b5cf6',
    type: 'action',
  },
  {
    id: 'classify',
    title: 'Classify',
    subtitle: 'By extension',
    Icon: IconSort,
    x: 62,
    y: 14,
    accent: '#a855f7',
    type: 'action',
  },
  {
    id: 'dryrun',
    title: 'Dry-run',
    subtitle: 'Preview moves',
    Icon: IconShield,
    x: 18,
    y: 54,
    accent: '#059669',
    type: 'action',
  },
  {
    id: 'organize',
    title: 'Organize',
    subtitle: 'Move to folders',
    Icon: IconFolder,
    x: 46,
    y: 54,
    accent: '#2563eb',
    type: 'action',
  },
  {
    id: 'report',
    title: 'PDF report',
    subtitle: 'Save summary',
    Icon: IconReport,
    x: 74,
    y: 54,
    accent: '#dc2626',
    type: 'output',
  },
]

const workflowEdges = [
  { from: 'folder', to: 'scan' },
  { from: 'scan', to: 'classify' },
  { from: 'classify', to: 'dryrun' },
  { from: 'dryrun', to: 'organize' },
  { from: 'organize', to: 'report' },
]

const categoryChips = [
  { label: 'Images', ext: '.jpg', Icon: IconImage, color: '#ec4899' },
  { label: 'Documents', ext: '.pdf', Icon: IconDocuments, color: '#f59e0b' },
  { label: 'Audio', ext: '.mp3', Icon: IconAudio, color: '#14b8a6' },
  { label: 'Video', ext: '.mp4', Icon: IconVideo, color: '#3b82f6' },
]

const NODE_W = 18
const NODE_H = 14

function nodeCenter(node) {
  return { x: node.x + NODE_W / 2, y: node.y + NODE_H / 2 }
}

function edgePath(from, to) {
  const a = nodeCenter(from)
  const b = nodeCenter(to)
  const dx = Math.abs(b.x - a.x)
  const curve = Math.max(dx * 0.45, 8)
  return `M ${a.x} ${a.y} C ${a.x + curve} ${a.y}, ${b.x - curve} ${b.y}, ${b.x} ${b.y}`
}

export default function Hero() {
  const [activeStep, setActiveStep] = useState(0)
  const totalSteps = workflowNodes.length

  useEffect(() => {
    const timer = setInterval(() => {
      setActiveStep((s) => (s + 1) % totalSteps)
    }, 1800)
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
              Step {activeStep + 1}/{totalSteps} ·{' '}
              {workflowNodes[activeStep]?.title}
            </span>
          </div>

          <div className="workflow-canvas">
            <svg className="workflow-edges" viewBox="0 0 100 72" preserveAspectRatio="none">
              <defs>
                <linearGradient id="edge-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
                  <stop offset="0%" stopColor="#6366f1" />
                  <stop offset="100%" stopColor="#2563eb" />
                </linearGradient>
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
                    d={edgePath(from, to)}
                    className={`workflow-edge ${isActive ? 'workflow-edge-done' : ''} ${isCurrent ? 'workflow-edge-active' : ''}`}
                    fill="none"
                    strokeWidth="0.55"
                    vectorEffect="non-scaling-stroke"
                  />
                )
              })}
            </svg>

            {workflowNodes.map((node, index) => {
              const Icon = node.Icon
              const isActive = node.id === activeNodeId
              const isDone = index < activeStep
              return (
                <div
                  key={node.id}
                  className={`workflow-node workflow-node-${node.type} ${isActive ? 'workflow-node-active' : ''} ${isDone ? 'workflow-node-done' : ''}`}
                  style={{ left: `${node.x}%`, top: `${node.y}%` }}
                >
                  <span className="workflow-port workflow-port-in" />
                  <div className="workflow-node-body">
                    <div className="workflow-node-icon" style={{ '--node-accent': node.accent }}>
                      <Icon />
                    </div>
                    <div className="workflow-node-text">
                      <span className="workflow-node-title">{node.title}</span>
                      <span className="workflow-node-sub">{node.subtitle}</span>
                    </div>
                  </div>
                  <span className="workflow-port workflow-port-out" />
                  {isActive && <span className="workflow-pulse" />}
                </div>
              )
            })}

            <div className="workflow-branch">
              <span className="workflow-branch-label">Routes to</span>
              <div className="workflow-chips">
                {categoryChips.map((chip) => {
                  const ChipIcon = chip.Icon
                  const lit = activeStep >= 2
                  return (
                    <span
                      key={chip.label}
                      className={`workflow-chip ${lit || activeStep >= 3 ? 'workflow-chip-lit' : ''}`}
                      style={{ '--chip-color': chip.color }}
                    >
                      <ChipIcon />
                      {chip.ext}
                    </span>
                  )
                })}
              </div>
            </div>
          </div>

          <div className="workflow-footer">
            <div className="workflow-progress">
              <div
                className="workflow-progress-fill"
                style={{ width: `${((activeStep + 1) / totalSteps) * 100}%` }}
              />
            </div>
            <span className="workflow-footer-text">
              {workflowNodes[activeStep]?.subtitle} — automated pipeline, fully offline
            </span>
          </div>
        </div>
      </div>
    </section>
  )
}
