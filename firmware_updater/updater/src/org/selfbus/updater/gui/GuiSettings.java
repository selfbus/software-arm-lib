package org.selfbus.updater.gui;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.databind.node.ObjectNode;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.swing.*;
import java.awt.*;
import java.io.File;
import java.io.IOException;

public class GuiSettings {
    private static final Logger logger = LoggerFactory.getLogger(GuiSettings.class);
    private final JFrame parent;
    private static final ObjectMapper objectMapper = new ObjectMapper()
            .enable(SerializationFeature.INDENT_OUTPUT)
            .enable(SerializationFeature.WRAP_ROOT_VALUE);

    @SuppressWarnings("unused")
    private GuiSettings() {
        this.parent = null;
    }

    private ObjectNode getObjectNode() {
        return objectMapper.createObjectNode();
    }

    public GuiSettings(JFrame parent) {
        this.parent = parent;
        logger.debug("parent: {} name: {}", this.parent.getClass().getName(), this.parent.getName());
    }

    public void readComponentsSettings(String fileName) throws IOException {
        logger.debug("reading '{}'", fileName);
        JsonNode rootNode = objectMapper.readTree(new File(fileName));
        readComponentData(parent, rootNode);
    }

    public void writeComponentSettings(String fileName) throws IOException {
        logger.debug("saving '{}'", fileName);
        JsonNode rootNode = writeComponentsData(parent);
        objectMapper.writeValue(new File(fileName), rootNode);
    }

    private void readComponentData(Component component, JsonNode node) {
        String nodeKey;
        // GuiMain.MainFrame is saved with json root key `ObjectNode`s simple class name
        // this.getObjectNode() ensures that we set the same nodeKey as in writeComponentsData()
        if (component == parent) {
            nodeKey = this.getObjectNode().getClass().getSimpleName();
        }
        else {
            nodeKey = getComponentSettingKey(component);
        }
        JsonNode componentNode = node.path(nodeKey);

        if (component instanceof JCheckBox checkBox) {
            checkBox.setSelected(componentNode.path("selected").asBoolean());
        }
        else if (component instanceof JTextField textField) {
            textField.setText(componentNode.path("text").asText());
        }
        else if (component instanceof JComboBox<?> comboBox) {
            int selectionIndex = componentNode.path("selectionIndex").asInt(-1);
            if ((selectionIndex >= 0) && (selectionIndex < comboBox.getItemCount())) {
                comboBox.setSelectedIndex(selectionIndex);
            }
        }
        else if (component instanceof Container container) {
            if (component instanceof JFrame frame) {
                readFrame(frame, componentNode);
            }
            for (Component child : container.getComponents()) {
                readComponentData(child, componentNode);
            }
        }
    }

    private JsonNode writeComponentsData(Component component) {
        ObjectNode node = getObjectNode();
        if (component instanceof JCheckBox checkBox) {
            node.put("selected", checkBox.isSelected());
        }
        else if (component instanceof JTextField textField) {
            node.put("text", textField.getText());
        }
        else if (component instanceof JComboBox<?> comboBox) {
            int selectionIndex = comboBox.getSelectedIndex();
            node.put("selectionIndex", selectionIndex);
        }
        else if (component instanceof Container container) {
            if (component instanceof JFrame frame) {
                writeFrame(frame, node);
            }
            for (Component child : container.getComponents()) {
                JsonNode childNode = writeComponentsData(child);
                if (!childNode.isEmpty(null)) {
                    node.set(getComponentSettingKey(child), childNode);
                }
            }
        }
        return node;
    }

    private String getComponentSettingKey(Component component) {
        String key = component.getClass().getSimpleName();
        if (component.getName() != null) {
            key += "." + component.getName();
        }
        else {
            key += ".null";
        }
        return key;
    }

    private void readFrame(JFrame frame, JsonNode node) {
        frame.setSize(node.path("width").asInt(), node.path("height").asInt());
        frame.setLocation(node.path("x").asInt(), node.path("y").asInt());
        frame.setExtendedState(node.path("extended_state").asInt());
    }

    private void writeFrame(JFrame frame, ObjectNode node) {
        node.put("width", frame.getWidth());
        node.put("height", frame.getHeight());
        node.put("x", frame.getX());
        node.put("y", frame.getY());
        node.put("extended_state", frame.getExtendedState());
    }
}