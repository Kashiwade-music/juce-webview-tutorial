import { FC, useEffect, useState } from "react";
import { Slider } from "antd";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";

interface JuceSliderProps {
  identifier: string;
  isVertical?: boolean;
}

const JuceSlider: FC<JuceSliderProps> = ({
  identifier,
  isVertical = false,
}) => {
  const sliderState = Juce.getSliderState(identifier);
  const [value, setValue] = useState<number>(sliderState.getNormalisedValue());

  const changeJUCEParamValue = (newNormalisedValue: number) => {
    sliderState.setNormalisedValue(newNormalisedValue);
  };

  useEffect(() => {
    const updateWebViewValue = () => {
      setValue(sliderState.getNormalisedValue());
    };

    const valueListenerId =
      sliderState.valueChangedEvent.addListener(updateWebViewValue);

    return () => {
      sliderState.valueChangedEvent.removeListener(valueListenerId);
    };
  }, [sliderState]);

  const style = isVertical ? {} : { width: "100%" };

  return (
    <Slider
      vertical={isVertical}
      min={0}
      max={1}
      step={0.001}
      value={value}
      onChange={(v) => {
        changeJUCEParamValue(v);
      }}
      tooltip={{ open: false }}
      style={style}
    />
  );
};

export default JuceSlider;
