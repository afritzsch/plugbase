// TODO: import controls and hooks from plugui as needed, e.g.:
// import { Knob, useJuceSlider } from 'plugui'
import { Waveform, Spectrum, useWaveform, useSpectrum } from 'plugui'

export default function App() {
  const samples = useWaveform('waveformUpdate')
  const bins    = useSpectrum('spectrumUpdate')

  return (
    <div className="app">
      <div className="plugin-header">
        <div className="plugin-title">My Plugin</div>
      </div>
      <div className="plugin-controls">
        <div style={{ display: 'flex', flexDirection: 'column', gap: 16, alignItems: 'center' }}>
          {/* TODO: add your controls here */}
          <Waveform samples={samples} width={340} height={80} />
          <Spectrum bins={bins}       width={340} height={80} />
        </div>
      </div>
    </div>
  )
}
