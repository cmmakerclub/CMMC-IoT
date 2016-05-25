MqttConnector::prepare_data_hook_t on_prepare_data =
[&](JsonObject * root) -> void {
    JsonObject& data = (*root)["d"];
    JsonObject& info = (*root)["info"];

    data["myName"] = DEVICE_NAME;
    data["millis"] = millis();
};