#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace vx;

namespace
{
struct PresetDef
{
    const char* name;
    int category;
    std::vector<std::pair<const char*, float>> values;
};

// Categorias: 0 CANTANTES, 1 TRAP, 2 REGGAETÓN, 3 R&B.
// Los presets de CANTANTES son perfiles creativos inspirados en las
// características de mezcla indicadas para cada voz; no pretenden ser
// emulaciones exactas de personas reales.
const std::vector<PresetDef>& presetList()
{
    static const std::vector<PresetDef> list
    {
        // CANTANTES --------------------------------------------------------
        { "Quevedo", 0,
          { { "compAmount", 58 }, { "compMode", 0 }, { "deEssAmount", 45 }, { "deEssFreq", 7000 },
            { "lowCut", 80 }, { "eqMid", -2.5f }, { "satAmount", 18 }, { "satType", 0 },
            { "magic", 18 }, { "airMid", 12 }, { "airHigh", 18 }, { "reverbMix", 12 }, { "reverbType", 0 },
            { "reverbLow", 180 }, { "reverbHigh", 8500 }, { "delayMix", 10 }, { "delayDiv", 1 },
            { "delayFb", 25 }, { "delayLow", 180 }, { "delayHigh", 6500 },
            { "tuneOn", 1 }, { "tuneSpeed", 18 }, { "tuneAmount", 92 }, { "tuneTol", 8 } } },

        // TRAP -------------------------------------------------------------
        { "Trap Moderno", 1,
          { { "compAmount", 66 }, { "compMode", 2 }, { "deEssAmount", 48 }, { "lowCut", 85 },
            { "eqMid", 1.0f }, { "satAmount", 24 }, { "satType", 2 }, { "magic", 12 },
            { "airMid", 16 }, { "airHigh", 20 }, { "reverbMix", 15 }, { "reverbType", 1 },
            { "delayMix", 22 }, { "delayDiv", 1 }, { "delayFb", 22 },
            { "tuneOn", 1 }, { "tuneSpeed", 7 }, { "tuneAmount", 100 }, { "tuneTol", 3 } } },

        { "Trap Oscuro", 1,
          { { "compAmount", 72 }, { "compMode", 2 }, { "deEssAmount", 42 }, { "lowCut", 78 },
            { "eqLow", -1.0f }, { "eqMid", 1.5f }, { "satAmount", 38 }, { "satType", 2 },
            { "magic", 8 }, { "airMid", 8 }, { "airHigh", 8 }, { "reverbMix", 10 }, { "reverbType", 1 },
            { "delayMix", 12 }, { "delayDiv", 0 }, { "delayFb", 14 },
            { "tuneOn", 1 }, { "tuneSpeed", 12 }, { "tuneAmount", 95 }, { "tuneTol", 5 } } },

        { "Trap Melódico", 1,
          { { "compAmount", 54 }, { "compMode", 1 }, { "deEssAmount", 40 }, { "lowCut", 82 },
            { "eqMid", 0.8f }, { "eqHigh", 1.5f }, { "satAmount", 15 }, { "satType", 0 },
            { "magic", 18 }, { "airMid", 24 }, { "airHigh", 28 }, { "reverbMix", 24 }, { "reverbType", 2 },
            { "delayMix", 26 }, { "delayDiv", 1 }, { "delayFb", 34 },
            { "tuneOn", 1 }, { "tuneSpeed", 15 }, { "tuneAmount", 96 }, { "tuneTol", 6 } } },

        // REGGAETÓN --------------------------------------------------------
        { "Reggaetón Limpio", 2,
          { { "compAmount", 50 }, { "compMode", 0 }, { "deEssAmount", 48 }, { "lowCut", 90 },
            { "eqHigh", 2.0f }, { "satAmount", 8 }, { "satType", 1 }, { "magic", 18 },
            { "airMid", 30 }, { "airHigh", 42 }, { "reverbMix", 18 }, { "reverbType", 0 },
            { "delayMix", 16 }, { "delayDiv", 1 }, { "delayFb", 28 },
            { "tuneOn", 1 }, { "tuneSpeed", 9 }, { "tuneAmount", 98 }, { "tuneTol", 4 } } },

        { "Reggaetón Amplio", 2,
          { { "compAmount", 46 }, { "compMode", 0 }, { "deEssAmount", 44 }, { "lowCut", 88 },
            { "eqMid", 0.8f }, { "eqHigh", 2.5f }, { "satAmount", 7 }, { "satType", 0 }, { "magic", 22 },
            { "airMid", 40 }, { "airHigh", 54 }, { "reverbMix", 28 }, { "reverbType", 2 },
            { "delayMix", 22 }, { "delayDiv", 2 }, { "delayFb", 34 },
            { "tuneOn", 1 }, { "tuneSpeed", 5 }, { "tuneAmount", 100 }, { "tuneTol", 2 } } },

        // R&B ----------------------------------------------------------------
        { "R&B Velvet", 3,
          { { "compAmount", 42 }, { "compMode", 0 }, { "deEssAmount", 32 }, { "deEssFreq", 6800 },
            { "lowCut", 78 }, { "eqMid", 2.0f }, { "eqHigh", 0.5f }, { "satAmount", 10 }, { "satType", 0 },
            { "magic", 20 }, { "airMid", 24 }, { "airHigh", 28 }, { "reverbMix", 32 }, { "reverbType", 2 },
            { "delayMix", 28 }, { "delayDiv", 1 }, { "delayFb", 38 },
            { "tuneOn", 1 }, { "tuneSpeed", 22 }, { "tuneAmount", 88 }, { "tuneTol", 8 } } },

        { "R&B Intimo", 3,
          { { "compAmount", 32 }, { "compMode", 0 }, { "deEssAmount", 28 }, { "lowCut", 75 },
            { "eqLow", 1.0f }, { "eqMid", 1.5f }, { "satAmount", 6 }, { "satType", 0 },
            { "magic", 12 }, { "airMid", 18 }, { "airHigh", 24 }, { "reverbMix", 20 }, { "reverbType", 1 },
            { "delayMix", 20 }, { "delayDiv", 1 }, { "delayFb", 30 },
            { "tuneOn", 1 }, { "tuneSpeed", 28 }, { "tuneAmount", 80 }, { "tuneTol", 10 } } },

        { "R&B Dream", 3,
          { { "compAmount", 38 }, { "compMode", 0 }, { "deEssAmount", 34 }, { "lowCut", 80 },
            { "eqMid", 1.5f }, { "eqHigh", 1.0f }, { "satAmount", 9 }, { "satType", 0 }, { "magic", 22 },
            { "airMid", 32 }, { "airHigh", 42 }, { "reverbMix", 42 }, { "reverbType", 2 },
            { "delayMix", 34 }, { "delayDiv", 1 }, { "delayFb", 45 },
            { "tuneOn", 1 }, { "tuneSpeed", 18 }, { "tuneAmount", 86 }, { "tuneTol", 7 } } }
    };
    return list;
}

const char* categoryName (int category)
{
    static constexpr const char* names[] = { "CANTANTES", "TRAP", "REGGAETÓN", "R&B" };
    return names[juce::jlimit (0, 3, category)];
}

void setParam (juce::AudioProcessorValueTreeState& s, const juce::String& id, float realValue)
{
    if (auto* p = s.getParameter (id))
        p->setValueNotifyingHost (p->convertTo0to1 (realValue));
}

float getParam (juce::AudioProcessorValueTreeState& s, const juce::String& id)
{
    return s.getRawParameterValue (id)->load();
}
} // namespace

//==============================================================================
int VoxoraProcessor::getNumCategories() const noexcept { return 4; }

juce::String VoxoraProcessor::getCategoryName (int category) const
{
    return categoryName (category);
}

juce::Array<int> VoxoraProcessor::getPresetIndicesForCategory (int category) const
{
    juce::Array<int> result;
    for (int i = 0; i < getNumPrograms(); ++i)
        if (presetList()[(size_t) i].category == category)
            result.add (i);
    return result;
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout VoxoraProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto addF = [&] (const char* id, const char* name, float lo, float hi, float def, float skew = 1.0f)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name,
                                                                 juce::NormalisableRange<float> (lo, hi, 0.01f, skew), def));
    };
    auto addC = [&] (const char* id, const char* name, juce::StringArray items, int def)
    {
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { id, 1 }, name, items, def));
    };

    addF ("inGain",  "Input Gain",  -24.0f, 24.0f, 0.0f);
    addF ("outGain", "Output Gain", -24.0f, 24.0f, 0.0f);

    addF ("lowCut", "Low Cut",  20.0f, 400.0f, 80.0f, 0.5f);
    addF ("eqLow",  "EQ Low",  -12.0f, 12.0f, 0.0f);
    addF ("eqMid",  "EQ Mid",  -12.0f, 12.0f, 0.0f);
    addF ("eqHigh", "EQ High", -12.0f, 12.0f, 0.0f);

    addF ("compAmount", "Comp Amount", 0.0f, 100.0f, 40.0f);
    addC ("compMode", "Comp Mode", { "Smooth", "Punchy", "Aggressive" }, 0);
    addF ("deEssAmount", "De-ess Amount", 0.0f, 100.0f, 30.0f);
    addF ("deEssFreq", "De-ess Focus", 3000.0f, 10000.0f, 6500.0f, 0.6f);

    addF ("satAmount", "Saturation", 0.0f, 100.0f, 0.0f);
    addC ("satType", "Saturation Type", { "Warm", "Agudo", "Saturacion" }, 0);
    addF ("magic", "Magic", 0.0f, 100.0f, 0.0f);

    addF ("airMid",  "Air Mid",  0.0f, 100.0f, 0.0f);
    addF ("airHigh", "Air High", 0.0f, 100.0f, 0.0f);

    // Mic character lives in ADVANCED only. The default is a gentle Manley-style tube voicing.
    addC ("micProfile", "Mic Profile", { "Manley-Style Tube", "Neumann TLM 103-Style", "AKG C414 XLII-Style", "RØDE NT1A-Style", "Audio-Technica AT2020-Style", "AKG P220-Style", "Sennheiser e835-Style", "Blue Yeti-Style" }, 0);
    addF ("micBody", "Mic Body", 0.0f, 100.0f, 0.0f);
    addF ("micAir", "Mic Air", 0.0f, 100.0f, 0.0f);

    addF ("reverbMix", "Reverb Mix", 0.0f, 100.0f, 0.0f);
    addC ("reverbType", "Reverb Type", { "Plate", "Room", "Large" }, 1);
    addF ("reverbLow",  "Reverb Low Cut",  20.0f, 1000.0f, 200.0f, 0.5f);
    addF ("reverbHigh", "Reverb High Cut", 2000.0f, 20000.0f, 9000.0f, 0.5f);

    addF ("delayMix", "Delay Mix", 0.0f, 100.0f, 0.0f);
    addC ("delayDiv", "Delay Time", { "1/16", "1/8", "1/4" }, 2);
    addF ("delayFb", "Delay Feedback", 0.0f, 90.0f, 35.0f);
    addF ("delayLow",  "Delay Low Cut",  20.0f, 1000.0f, 200.0f, 0.5f);
    addF ("delayHigh", "Delay High Cut", 2000.0f, 20000.0f, 7000.0f, 0.5f);

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "tuneOn", 1 }, "Tune On", false));
    addC ("tuneKey", "Tune Key", { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 0);
    addC ("tuneScale", "Tune Scale", { "Chromatic", "Major", "Minor", "Pentatonic Minor" }, 0);
    addF ("tuneSpeed", "Tune Speed", 0.0f, 200.0f, 15.0f);
    addF ("tuneAmount", "Tune Amount", 0.0f, 100.0f, 100.0f);
    addF ("tuneTol", "Tune Tolerance", 0.0f, 50.0f, 0.0f);

    return layout;
}

//==============================================================================
VoxoraProcessor::VoxoraProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "VOCODEXPRO", createLayout())
{
}

bool VoxoraProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return out == layouts.getMainInputChannelSet();
}

void VoxoraProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    const juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 2 };

    eq.prepare (sampleRate);
    micEmulator.prepare (sampleRate);
    comp.prepare (sampleRate);
    deEss.prepare (sampleRate);
    sat.prepare (sampleRate);
    air.prepare (sampleRate);
    reverb.prepare (spec);
    delay.prepare (sampleRate);
    detector.prepare (sampleRate);
    shifter.prepare (sampleRate);
    assistHp.prepare (sampleRate);

    dryCopy.setSize (2, samplesPerBlock);
    prevIn = vx::dbToGain (getParam (apvts, "inGain"));
    prevOut = vx::dbToGain (getParam (apvts, "outGain"));
    correction = desired = 0.0f;
    voiced = false;

    setLatencySamples (shifter.getLatency());
}

//==============================================================================
void VoxoraProcessor::processBlock (juce::AudioBuffer<float>& buf, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int n = buf.getNumSamples();
    const int nch = juce::jmin (2, buf.getNumChannels());
    for (int c = getTotalNumInputChannels(); c < buf.getNumChannels(); ++c)
        buf.clear (c, 0, n);
    if (n == 0 || nch == 0) return;

    auto P = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };

    double bpm = 120.0;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto b = pos->getBpm())
                bpm = *b;

    // ---- Entrada ----
    const float inG = vx::dbToGain (P ("inGain"));
    buf.applyGainRamp (0, n, prevIn, inG);
    prevIn = inG;
    inPeak.store (std::max (inPeak.load(), buf.getMagnitude (0, n)));

    if (assistState.load() == assistListening)
        analyseForAssist (buf);

    // ---- Correccion de tono (siempre en la ruta para mantener la latencia constante) ----
    {
        const bool tuneOn = P ("tuneOn") > 0.5f;
        const int key = (int) P ("tuneKey");
        const int scale = (int) P ("tuneScale");
        const float speed = P ("tuneSpeed");
        const float amount = P ("tuneAmount") * 0.01f;
        const float tolCents = P ("tuneTol");

        static const bool masks[4][12] = {
            { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },   // Chromatic
            { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 },   // Major
            { 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0 },   // Minor
            { 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0 }    // Pentatonic Minor
        };
        const bool* mask = masks[juce::jlimit (0, 3, scale)];

        const float coeff = speed < 0.5f ? 1.0f : 1.0f - std::exp (-1.0f / (speed * 0.001f * (float) sr));
        const int holdSamples = (int) (0.08 * sr);

        float* ch0 = buf.getWritePointer (0);
        float* ch1 = nch > 1 ? buf.getWritePointer (1) : nullptr;

        for (int i = 0; i < n; ++i)
        {
            float frame[2] = { ch0[i], ch1 != nullptr ? ch1[i] : 0.0f };

            if (tuneOn)
            {
                const float mono = ch1 != nullptr ? 0.5f * (frame[0] + frame[1]) : frame[0];
                float f0 = 0.0f;
                if (detector.push (mono, f0))
                {
                    if (f0 > 0.0f)
                    {
                        voiced = true; unvoicedSamples = 0; lastF0 = f0;

                        const float midi = 69.0f + 12.0f * std::log2 (f0 / 440.0f);
                        const int centre = (int) std::lround (midi);
                        float best = (float) centre, bestDist = 1.0e9f;
                        for (int k = -6; k <= 6; ++k)
                        {
                            const int note = centre + k;
                            const int pc = (((note - key) % 12) + 12) % 12;
                            if (! mask[pc]) continue;
                            const float dist = std::abs ((float) note - midi);
                            if (dist < bestDist) { bestDist = dist; best = (float) note; }
                        }

                        float diff = best - midi;
                        if (std::abs (diff) * 100.0f < tolCents) diff = 0.0f;
                        desired = diff * amount;
                        detectedMidi.store (midi);
                    }
                    else
                    {
                        voiced = false;
                        detectedMidi.store (-1.0f);
                    }
                }

                if (! voiced)
                {
                    if (++unvoicedSamples > holdSamples) desired = 0.0f;
                }

                correction += coeff * (desired - correction);
            }
            else
            {
                correction = desired = 0.0f;
            }

            const float ratio = std::exp2 (correction / 12.0f);
            const float mixT = (tuneOn && std::abs (correction) > 0.01f) ? 1.0f : 0.0f;
            shifter.processFrame (frame, nch, ratio, mixT);

            ch0[i] = frame[0];
            if (ch1 != nullptr) ch1[i] = frame[1];
        }

        if (! tuneOn) detectedMidi.store (-1.0f);
    }

    // ---- Color de microfono (ADVANCED only) ----
    micEmulator.process (buf, (int) P ("micProfile"), P ("micBody"), P ("micAir"));

    // ---- Tono / dinamica ----
    const float magic = P ("magic");
    eq.process (buf, P ("lowCut"), P ("eqLow"), P ("eqMid"), P ("eqHigh") + magic * 0.045f);

    comp.process (buf, P ("compAmount"), (int) P ("compMode"));
    grMeter.store (std::max (grMeter.load(), comp.grOut.load()));

    deEss.process (buf, P ("deEssAmount"), P ("deEssFreq"));
    sat.process (buf, juce::jlimit (0.0f, 100.0f, P ("satAmount") + magic * 0.2f), (int) P ("satType"));
    air.process (buf, P ("airMid"), P ("airHigh"));

    // ---- Espacio (reverb y delay en paralelo) ----
    if (dryCopy.getNumSamples() < n) dryCopy.setSize (2, n, false, false, true);
    for (int ch = 0; ch < nch; ++ch)
        dryCopy.copyFrom (ch, 0, buf, ch, 0, n);

    juce::AudioBuffer<float> dryView (dryCopy.getArrayOfWritePointers(), nch, n);

    reverb.process (dryView, buf, P ("reverbMix"), (int) P ("reverbType"), P ("reverbLow"), P ("reverbHigh"));
    delay.process (dryView, buf, P ("delayMix"), (int) P ("delayDiv"), P ("delayFb"), P ("delayLow"), P ("delayHigh"), bpm);

    // ---- Salida ----
    const float outG = vx::dbToGain (P ("outGain"));
    buf.applyGainRamp (0, n, prevOut, outG);
    prevOut = outG;
    outPeak.store (std::max (outPeak.load(), buf.getMagnitude (0, n)));
}

//==============================================================================
// Asistente vocal
void VoxoraProcessor::startAssist()
{
    assistProgress.store (0.0f);
    assistResetRequest.store (true);
    assistState.store (assistListening);
}

void VoxoraProcessor::analyseForAssist (const juce::AudioBuffer<float>& buf)
{
    if (assistResetRequest.exchange (false))
    {
        aSumSq = aSumHigh = 0.0; aPeak = 0.0f; aCount = 0;
    }

    assistHp.set (StereoBiquad::Type::highPass, 5000.0f, 0.707f);
    const int nch = juce::jmin (2, buf.getNumChannels());
    const long long target = (long long) (6.0 * sr);

    for (int i = 0; i < buf.getNumSamples(); ++i)
    {
        float m = 0.0f;
        for (int ch = 0; ch < nch; ++ch) m += buf.getSample (ch, i);
        m /= (float) nch;

        const float hp = assistHp.process (0, m);
        if (std::abs (m) > 0.003f)       // solo cuenta lo que suena (puerta ~ -50 dB)
        {
            aSumSq += (double) m * m;
            aSumHigh += (double) hp * hp;
            aPeak = std::max (aPeak, std::abs (m));
            ++aCount;
        }
    }

    assistProgress.store ((float) juce::jmin (1.0, (double) aCount / (double) target));

    if (aCount >= target)
    {
        const float rmsDb = (float) (10.0 * std::log10 (aSumSq / (double) aCount + 1.0e-12));
        const float peakDb = vx::gainToDb (aPeak);
        const float crest = peakDb - rmsDb;
        const float sib = (float) (aSumHigh / (aSumSq + 1.0e-12));

        assistResult[0].store (juce::jlimit (-12.0f, 12.0f, -18.0f - rmsDb));      // ajuste de ganancia
        assistResult[1].store (juce::jlimit (25.0f, 75.0f, 20.0f + (crest - 12.0f) * 5.0f));   // compresion
        assistResult[2].store (juce::jlimit (15.0f, 70.0f, sib * 350.0f));          // de-esser
        assistState.store (assistComputed);
    }
}

void VoxoraProcessor::commitAssist()
{
    if (assistState.load() != assistComputed) return;

    static const char* ids[] = { "inGain", "compAmount", "deEssAmount", "lowCut", "eqMid", "eqHigh" };

    assistBefore.clear(); assistAfter.clear();
    for (auto* id : ids) assistBefore.push_back ({ id, getParam (apvts, id) });

    const float newIn = juce::jlimit (-24.0f, 24.0f, getParam (apvts, "inGain") + assistResult[0].load());
    const float vals[] = { newIn, assistResult[1].load(), assistResult[2].load(), 80.0f, 1.5f, 1.5f };
    for (size_t i = 0; i < std::size (ids); ++i) assistAfter.push_back ({ ids[i], vals[i] });

    for (auto& [id, v] : assistAfter) setParam (apvts, id, v);
    assistState.store (assistApplied);
}

void VoxoraProcessor::setAssistApplied (bool applied)
{
    if (assistBefore.empty()) return;
    for (auto& [id, v] : (applied ? assistAfter : assistBefore)) setParam (apvts, id, v);
    assistState.store (applied ? assistApplied : assistRestored);
}

//==============================================================================
// Presets
int VoxoraProcessor::getNumPrograms() { return (int) presetList().size(); }

const juce::String VoxoraProcessor::getProgramName (int index)
{
    if (index < 0 || index >= getNumPrograms()) return {};
    return presetList()[(size_t) index].name;
}

void VoxoraProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= getNumPrograms()) return;
    currentPreset = index;
    applyPreset (index);
}

void VoxoraProcessor::applyPreset (int index)
{
    // Primero valores por defecto (menos ganancias y tune), luego los del preset
    for (auto* p : getParameters())
    {
        if (auto* r = dynamic_cast<juce::RangedAudioParameter*> (p))
        {
            const auto id = r->paramID;
            const bool keepTune = id.startsWith ("tune") && id != "tuneOn" && id != "tuneSpeed" && id != "tuneAmount";
            if (id == "inGain" || id == "outGain" || keepTune)
                continue;
            r->setValueNotifyingHost (r->getDefaultValue());
        }
    }

    for (auto& [id, v] : presetList()[(size_t) index].values)
        setParam (apvts, id, v);
}

//==============================================================================
void VoxoraProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, dest);
}

void VoxoraProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* VoxoraProcessor::createEditor() { return new VoxoraEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VoxoraProcessor(); }
