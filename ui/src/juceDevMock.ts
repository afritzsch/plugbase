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
      emitEvent(id, payload) {
        if (payload?.eventType !== 'requestInitialUpdate') return

        // TODO: respond with mock properties and values for your parameters, e.g.:
        // if (id === 'gainSlider') {
        //   const cbs = [...(listeners[id]?.values() ?? [])]
        //   setTimeout(() => {
        //     cbs.forEach(cb => cb({ eventType: 'propertiesChanged', start: -60, end: 12, ... }))
        //     cbs.forEach(cb => cb({ eventType: 'valueChanged', value: 0.0 }))
        //   }, 0)
        // }
      },
    },
  }
}

export {}
