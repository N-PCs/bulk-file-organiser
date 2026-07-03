import './HeroBg.css'

const orbs = [
  { size: 500, x: 75, y: 20, color: 'var(--primary)', blur: 80, dur: 7 },
  { size: 300, x: 15, y: 65, color: 'var(--accent-soft)', blur: 60, dur: 9 },
  { size: 200, x: 55, y: 80, color: 'var(--primary)', blur: 50, dur: 11 },
]

const shapes = [
  { type: 'hex', x: 80, y: 18, size: 48, rotate: 15, dur: 12, delay: 0 },
  { type: 'diamond', x: 12, y: 55, size: 32, rotate: 0, dur: 15, delay: 1 },
  { type: 'circle', x: 65, y: 72, size: 24, rotate: 0, dur: 10, delay: 2 },
  { type: 'hex', x: 25, y: 20, size: 28, rotate: -20, dur: 14, delay: 0.5 },
  { type: 'diamond', x: 88, y: 50, size: 20, rotate: 45, dur: 11, delay: 1.5 },
  { type: 'circle', x: 45, y: 12, size: 16, rotate: 0, dur: 13, delay: 3 },
]

const nodes = [
  { x: 70, y: 25 },
  { x: 82, y: 35 },
  { x: 78, y: 55 },
  { x: 90, y: 48 },
  { x: 60, y: 65 },
  { x: 75, y: 78 },
]

const edges = [
  [0, 1], [1, 2], [2, 4], [1, 3], [3, 5], [4, 5],
]

const mobileDots = [
  { x: 12, y: 20, size: 3, delay: 0 },
  { x: 88, y: 35, size: 2, delay: 0.8 },
  { x: 25, y: 65, size: 4, delay: 1.6 },
  { x: 75, y: 80, size: 2, delay: 2.4 },
  { x: 50, y: 45, size: 3, delay: 0.4 },
  { x: 35, y: 30, size: 2, delay: 1.2 },
  { x: 65, y: 55, size: 3, delay: 2.0 },
  { x: 18, y: 85, size: 2, delay: 2.8 },
]

export default function HeroBg() {
  return (
    <div className="hero-bg" aria-hidden="true">
      {/* Desktop */}
      <div className="hero-bg-desktop">
        {orbs.map((o, i) => (
          <div
            key={i}
            className="hero-orb"
            style={{
              width: o.size,
              height: o.size,
              left: `${o.x}%`,
              top: `${o.y}%`,
              background: `radial-gradient(circle, ${o.color} 0%, transparent 70%)`,
              filter: `blur(${o.blur}px)`,
              animationDuration: `${o.dur}s`,
            }}
          />
        ))}

        <svg className="hero-network" viewBox="0 0 100 100" preserveAspectRatio="none">
          {edges.map(([a, b], i) => (
            <line
              key={i}
              x1={nodes[a].x} y1={nodes[a].y}
              x2={nodes[b].x} y2={nodes[b].y}
              className="hero-net-edge"
            />
          ))}
          {nodes.map((n, i) => (
            <circle
              key={i}
              cx={n.x} cy={n.y}
              className="hero-net-node"
              style={{ animationDelay: `${i * 0.7}s` }}
            />
          ))}
        </svg>

        {shapes.map((s, i) => (
          <div
            key={i}
            className={`hero-shape hero-shape-${s.type}`}
            style={{
              left: `${s.x}%`,
              top: `${s.y}%`,
              width: s.size,
              height: s.size,
              '--shape-rotate': `${s.rotate}deg`,
              animationDuration: `${s.dur}s`,
              animationDelay: `${s.delay}s`,
            }}
          />
        ))}
      </div>

      {/* Mobile */}
      <div className="hero-bg-mobile">
        <div className="hero-mobile-mesh" />
        <div className="hero-mobile-grid" />

        {mobileDots.map((d, i) => (
          <span
            key={i}
            className="hero-mobile-dot"
            style={{
              left: `${d.x}%`,
              top: `${d.y}%`,
              width: d.size,
              height: d.size,
              animationDelay: `${d.delay}s`,
            }}
          />
        ))}

        <div className="hero-mobile-line hero-mobile-line-1" />
        <div className="hero-mobile-line hero-mobile-line-2" />
      </div>

      <div className="hero-grain" />
    </div>
  )
}
