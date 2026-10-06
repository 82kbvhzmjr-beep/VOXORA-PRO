#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

namespace ui
{
const juce::Colour bg      { 0xff000000 };
const juce::Colour panel   { 0xff000000 };
const juce::Colour panel2  { 0xff000000 };
const juce::Colour accent  { 0xffffffff };
const juce::Colour accent2 { 0xffffffff };
const juce::Colour text    { 0xffffffff };
const juce::Colour dim     { 0xffffffff };
const juce::Colour warn    { 0xffffffff };
const juce::Colour value   { 0xffd4af37 };
}

//==============================================================================
class VoxoraLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VoxoraLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
};

//==============================================================================
class Knob : public juce::Component
{
public:
    Knob (juce::AudioProcessorValueTreeState& state, const juce::String& id, const juce::String& title,
          const juce::String& suffix = {}, int decimals = 1);
    void resized() override;

    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};

class Choice : public juce::Component
{
public:
    Choice (juce::AudioProcessorValueTreeState& state, const juce::String& id, const juce::String& title);
    void resized() override;

    juce::Label label;
    juce::ComboBox box;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
};

class LevelMeter : public juce::Component
{
public:
    explicit LevelMeter (bool isGainReduction = false) : gr (isGainReduction) {}
    void setLevel (float newLevel) { level = newLevel; repaint(); }
    void paint (juce::Graphics&) override;

private:
    float level = 0.0f;   // lineal (0-1) o dB de reduccion si gr == true
    bool gr;
};

//==============================================================================
class AssistPage : public juce::Component
{
public:
    AssistPage();
    void resized() override;
    void paint (juce::Graphics&) override;
    void refresh (int state, float progress);

    juce::TextButton startBtn { "Iniciar" }, originalBtn { "Original" }, assistedBtn { "Asistida" };

private:
    int state = 0;
    float progress = 0.0f;
};

class SimplePage : public juce::Component
{
public:
    explicit SimplePage (juce::AudioProcessorValueTreeState& s);
    void resized() override;
    void paint (juce::Graphics&) override;

private:
    Knob comp, magic, reverb, delay;
};

class AdvancedPage : public juce::Component
{
public:
    explicit AdvancedPage (juce::AudioProcessorValueTreeState& s);
    void resized() override;
    void paint (juce::Graphics&) override;
    void setNote (const juce::String& s) { noteLabel.setText (s, juce::dontSendNotification); }
    void setGr (float db) { grBar.setLevel (db); }

private:
    Knob lowCut, eqLow, eqMid, eqHigh;
    Knob comp, deEss, deEssFreq;
    Choice compMode;
    LevelMeter grBar { true };

    juce::ToggleButton tuneOn { "ACTIVO" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tuneOnAtt;
    Choice tuneKey, tuneScale;
    Knob tuneSpeed, tuneAmount, tuneTol;
    juce::Label noteLabel, noteTitle;

    Choice satType;
    Knob satAmount, magic;
    Knob airMid, airHigh;

    Choice micProfile;
    Knob micBody, micAir;

    Choice reverbType;
    Knob reverbMix, reverbLow, reverbHigh;
    Choice delayDiv;
    Knob delayMix, delayFb, delayLow, delayHigh;

    juce::Rectangle<int> secEq, secDyn, secTune, secTone, secAir, secMic, secReverb, secDelay;
};

//==============================================================================
class VoxoraEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit VoxoraEditor (VoxoraProcessor&);
    ~VoxoraEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void showPage (int index);
    void selectPreset (int index);
    void rebuildPresetListForCategory();

    VoxoraProcessor& proc;
    VoxoraLookAndFeel laf;

    juce::TextButton tabAssist { "ASSIST" }, tabSimple { "SIMPLE" }, tabAdvanced { "ADVANCED" };
    juce::ToggleButton tuneQuick { "TUNE" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tuneQuickAtt;

    juce::TextButton prevBtn { "<" }, nextBtn { ">" };
    juce::ComboBox categoryBox, presetBox;
    int currentCategory = 0;
    juce::Array<int> visiblePresetIndices;

    juce::Label inLabel { {}, "INPUT" }, outLabel { {}, "OUTPUT" };
    LevelMeter inMeter, outMeter;
    juce::Slider inSlider, outSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inAtt, outAtt;
    float inShown = 0.0f, outShown = 0.0f;

    AssistPage assistPage;
    SimplePage simplePage;
    AdvancedPage advancedPage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxoraEditor)
};
