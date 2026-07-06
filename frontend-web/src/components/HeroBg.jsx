import { useEffect, useRef } from 'react'
import './HeroBg.css'

export default function HeroBg() {
  const canvasRef = useRef(null)

  useEffect(() => {
    const canvas = canvasRef.current
    if (!canvas) return

    const ctx = canvas.getContext('2d')
    let animId

    const GRID = 18
    const cols = Math.ceil(window.innerWidth / GRID)
    const rows = Math.ceil(window.innerHeight / GRID)

    // Pre-compute a dither grid with random intensities
    const ditherGrid = Array.from({ length: rows }, () =>
      Array.from({ length: cols }, () => Math.random())
    )

    function resize() {
      canvas.width = window.innerWidth
      canvas.height = window.innerHeight
    }

    resize()
    window.addEventListener('resize', resize)

    // Parse the primary color to RGB
    const style = getComputedStyle(document.documentElement)
    const hex = style.getPropertyValue('--primary').trim()
    const r = parseInt(hex.slice(1, 3), 16)
    const g = parseInt(hex.slice(3, 5), 16)
    const b = parseInt(hex.slice(5, 7), 16)

    let time = 0

    function draw() {
      ctx.clearRect(0, 0, canvas.width, canvas.height)
      time += 0.04

      for (let y = 0; y < rows; y++) {
        for (let x = 0; x < cols; x++) {
          const base = ditherGrid[y][x]
          const offset = Math.sin(time + x * 0.5 + y * 0.35) * 0.35
            + Math.sin(time * 1.8 + x * 0.2 - y * 0.3) * 0.25
            + Math.sin(time * 2.5 - x * 0.15 + y * 0.4) * 0.2

          const threshold = base + offset

          if (threshold > 0.65) {
            const intensity = (threshold - 0.65) / 0.35
            const alpha = Math.min(intensity * 0.35, 0.35)

            ctx.fillStyle = `rgba(${r},${g},${b},${alpha})`
            ctx.fillRect(x * GRID, y * GRID, GRID - 1, GRID - 1)
          }
        }
      }

      animId = requestAnimationFrame(draw)
    }

    draw()

    return () => {
      cancelAnimationFrame(animId)
      window.removeEventListener('resize', resize)
    }
  }, [])

  return (
    <div className="hero-bg" aria-hidden="true">
      <canvas ref={canvasRef} className="hero-dither" />
      <div className="hero-mobile-bg">
        <div className="wave-left wave-1" />
        <div className="wave-left wave-2" />
        <div className="wave-left wave-3" />
        <div className="wave-right wave-1" />
        <div className="wave-right wave-2" />
        <div className="wave-right wave-3" />
      </div>
      <div className="hero-grain" />
    </div>
  )
}
