#pragma once
// This header intentionally left minimal as the existing registry and
// AVS_REGISTER_NODE macro are already present in the repository as per context.
// Plugins should simply call AVS_REGISTER_NODE(MyNodeClass) inside
// avs_plugin_register(). If a runtime registration API becomes necessary,
// extend this header in a future thread.
