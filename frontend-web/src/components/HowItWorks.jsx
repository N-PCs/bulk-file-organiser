import './HowItWorks.css'

const steps = [
  {
    num: '01',
    title: 'Pick a folder',
    description: 'Launch urFileManager and select any cluttered directory — Downloads, Desktop, or a project folder.',
    code: 'organizer.exe → Browse folder',
  },
  {
    num: '02',
    title: 'Preview with dry-run',
    description: 'Review the planned moves in the live log console. Disable dry-run when you are ready to commit.',
    code: 'Dry-run mode (default: ON)',
  },
  {
    num: '03',
    title: 'Organize and report',
    description: 'Start organizing. Files sort into subfolders instantly. View the in-app summary or export a PDF report.',
    code: 'organization_report.pdf',
  },
]

export default function HowItWorks() {
  return (
    <section id="how-it-works" className="section how-it-works">
      <div className="container">
        <div className="how-header">
          <span className="section-label">How it works</span>
          <h2 className="section-title">Three steps to a clean folder</h2>
        </div>

        <div className="steps">
          {steps.map((step) => (
            <div key={step.num} className="step card">
              <span className="step-num">{step.num}</span>
              <h3 className="step-title">{step.title}</h3>
              <p className="step-desc">{step.description}</p>
              <code className="step-code">{step.code}</code>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}
