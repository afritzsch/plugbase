// Browser-mode mock of the JUCE backend.
// Update initialisationData and emitEvent when you add parameters.

interface JuceEvent {
  eventType: string
  [key: string]: unknown
}

type EventCallback = (event: JuceEvent) => void

declare global {
  interface Window {
    __JUCE__?: {
      initialisationData: {
        __juce__sliders:    string[]
        __juce__toggles:    string[]
        __juce__comboBoxes: string[]
        __juce__functions:  string[]
        __juce__platform:   string[]
      }
      backend: {
        addEventListener(id: string, cb: EventCallback): [string, number]
        removeEventListener(id: [string, number]): void
        emitEvent(id: string, payload: JuceEvent): void
      }
    }
  }
}

if (!window.__JUCE__) {
  const listeners: Record<string, Map<number, EventCallback>> = {}
  let nextId = 0

  window.__JUCE__ = {
    initialisationData: {
      __juce__sliders:    [], // TODO: add your slider IDs, e.g. ['gainSlider']
      __juce__toggles:    [], // TODO: add your toggle IDs, e.g. ['bypassToggle']
      __juce__comboBoxes: [],
      __juce__functions:  [],
      __juce__platform:   [],
    },
    backend: {
      addEventListener(id, cb) {
        if (!listeners[id]) listeners[id] = new Map()
        const listenerId = nextId++
        listeners[id].set(listenerId, cb)
        return [id, listenerId]
      },
      removeEventListener([id, listenerId]) {
        listeners[id]?.delete(listenerId)
      },
      emitEvent(_id, _payload) {
        // TODO: handle requestInitialUpdate for your parameters
      },
    },
  }

  // Simulate waveform at 30 Hz
  let t = 0
  setInterval(() => {
    t += 0.05
    const samples = Array.from({ length: 256 }, (_, i) =>
      Math.sin(2 * Math.PI * (i / 256 + t)) * 0.5
    )
    const cbs = [...(listeners['waveformUpdate']?.values() ?? [])]
    cbs.forEach(cb => cb({ eventType: 'waveformUpdate', samples } as never))
  }, 33)

  // Simulate spectrum at 30 Hz
  setInterval(() => {
    const bins = Array.from({ length: 128 }, (_, i) =>
      -20 - (i / 128) * 60 + (Math.random() * 6 - 3)
    )
    const cbs = [...(listeners['spectrumUpdate']?.values() ?? [])]
    cbs.forEach(cb => cb({ eventType: 'spectrumUpdate', bins } as never))
  }, 33)
}

export {}
