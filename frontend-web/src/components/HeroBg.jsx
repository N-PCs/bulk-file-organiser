import './HeroBg.css'

const auroraLayers = [
  { color: 'var(--primary)', x: 20, y: 30, w: 50, h: 50, blur: 120, dur: 8, delay: 0 },
  { color: 'var(--accent-soft)', x: 70, y: 50, w: 40, h: 40, blur: 100, dur: 11, delay: 1 },
  { color: 'var(--primary)', x: 50, y: 80, w: 35, h: 35, blur: 90, dur: 9, delay: 2 },
  { color: '#6366f1', x: 10, y: 60, w: 30, h: 30, blur: 80, dur: 12, delay: 0.5 },
  { color: 'var(--primary)', x: 80, y: 15, w: 25, h: 25, blur: 70, dur: 10, delay: 1.5 },
]

const particles = Array.from({ length: 30 }, (_, i) => ({
  x: ((i * 17) % 100),
  y: ((i * 31 + 7) % 100),
  s: 1 + (i % 4),
  dur: 4 + (i % 5) * 2,
  delay: (i * 0.3) % 6,
  o: 0.15 + (i % 5) * 0.05,
}))

const gridLines = [
  { x: 25, h: 70 },
  { x: 50, h: 55 },
  { x: 75, h: 80 },
  { x: 15, h: 40 },
  { x: 85, h: 60 },
]

export default function HeroBg() {
  return (
    <div className="hero-bg" aria-hidden="true">
      <div className="hero-bg-desktop">
        {auroraLayers.map((l, i) => (
          <div
            key={i}
            className="hero-aurora"
            style={{
              left: `${l.x}%`,
              top: `${l.y}%`,
              width: `${l.w}%`,
              height: `${l.h}%`,
              background: `radial-gradient(ellipse at center, ${l.color} 0%, transparent 70%)`,
              filter: `blur(${l.blur}px)`,
              animationDuration: `${l.dur}s`,
              animationDelay: `${l.delay}s`,
            }}
          />
        ))}

        <svg className="hero-dot-grid" viewBox="0 0 100 100" preserveAspectRatio="none">
          <pattern id="dotPattern" x="0" y="0" width="4" height="4" patternUnits="userSpaceOnUse">
            <circle cx="2" cy="2" r="0.4" fill="var(--border-strong)" opacity="0.4" />
          </pattern>
          <rect width="100" height="100" fill="url(#dotPattern)" />
        </svg>

        <div className="hero-grid-mask" />

        {gridLines.map((l, i) => (
          <div
            key={i}
            className="hero-grid-line"
            style={{
              left: `${l.x}%`,
              height: `${l.h}%`,
              animationDelay: `${i * 1.2}s`,
            }}
          />
        ))}

        {particles.map((p, i) => (
          <span
            key={i}
            className="hero-particle"
            style={{
              left: `${p.x}%`,
              top: `${p.y}%`,
              width: p.s,
              height: p.s,
              opacity: p.o,
              animationDuration: `${p.dur}s`,
              animationDelay: `${p.delay}s`,
            }}
          />
        ))}

        <div className="hero-accent-line hero-accent-line-1" />
        <div className="hero-accent-line hero-accent-line-2" />
        <div className="hero-accent-line hero-accent-line-3" />
      </div>

      <div className="hero-bg-mobile">
        <div className="hero-mobile-mesh" />
        <div className="hero-mobile-grid" />
      </div>

      <div className="hero-grain" />
    </div>
  )
}
