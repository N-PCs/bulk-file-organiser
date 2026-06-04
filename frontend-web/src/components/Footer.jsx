import './Footer.css'

export default function Footer() {
  return (
    <footer className="footer">
      <div className="container footer-inner">
        <div className="footer-brand">
          <span className="footer-logo">urFM</span>
          <p className="footer-tagline">Smart file organization for all platforms.</p>
        </div>

        <div className="footer-links">
          <a href="#features">Features</a>
          <a href="#how-it-works">How it works</a>
          <a href="#themes">Themes</a>
          <a href="#download">Download</a>
        </div>

        <p className="footer-copy">
          © {new Date().getFullYear()} urFileManager. Native C++, fully offline, no tracking.
        </p>
      </div>
    </footer>
  )
}
