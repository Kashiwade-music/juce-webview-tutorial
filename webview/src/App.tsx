import { FC } from "react";
import { Layout, Flex, Typography } from "antd";
import JuceSlider from "./components/JuceSlider";
import JuceTextbox from "./components/JuceTextbox";
import JuceCombobox from "./components/JuceCombobox";
import JuceToggleSwitch from "./components/JuceToggleSwitch";

const App: FC = () => {
  return (
    <Layout>
      <Layout.Content>
        <Flex
          vertical
          gap="middle"
          justify="space-between"
          align="center"
          style={{ padding: "16px", height: "100vh" }}
        >
          <Typography.Title
            level={3}
            style={{ margin: 0, textAlign: "center" }}
          >
            Pan
          </Typography.Title>
          <JuceSlider identifier="panAngle" />
          <JuceTextbox identifier="panAngle" digits={0} />
          <JuceCombobox identifier="panRule" />
          <Typography.Title
            level={3}
            style={{ margin: 0, textAlign: "center" }}
          >
            Gain
          </Typography.Title>
          <JuceSlider identifier="gain" isVertical={true} />
          <JuceTextbox identifier="gain" suffix="dB" />
          <JuceToggleSwitch identifier="bypass" inverted={true} />
        </Flex>
      </Layout.Content>
    </Layout>
  );
};

export default App;
