import { FC, useEffect, useState } from "react";
import { Switch } from "antd";
// @ts-expect-error Juce does not have types
import * as Juce from "juce-framework-frontend";

interface JuceToggleSwitchProps {
  identifier: string;
  inverted?: boolean;
}

const JuceToggleSwitch: FC<JuceToggleSwitchProps> = ({
  identifier,
  inverted = false,
}) => {
  const toggleState = Juce.getToggleState(identifier);
  const [value, setValue] = useState(toggleState.getValue());

  const changeJUCEParamValue = (newValue: boolean) => {
    toggleState.setValue(newValue);
  };

  useEffect(() => {
    const updateWebViewValue = () => {
      setValue(toggleState.getValue());
    };

    const valueListenerId =
      toggleState.valueChangedEvent.addListener(updateWebViewValue);

    return () => {
      toggleState.valueChangedEvent.removeListener(valueListenerId);
    };
  }, [toggleState, inverted]);

  return (
    <Switch
      value={inverted ? !value : value}
      onChange={(v) => changeJUCEParamValue(inverted ? !v : v)}
    />
  );
};

export default JuceToggleSwitch;
