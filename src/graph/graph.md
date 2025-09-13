
# AutoVisionStudio (AVS) - Architecture

## Overview

AutoVisionStudio (AVS) is a modular, high-performance application built in C++17, aimed at providing a no-code environment for robotics and vision systems. The application is designed to be scalable, leveraging a node-based graph structure for defining workflows and execution sequences. The components are modularized, allowing them to be built, debugged, and extended independently.

## Key Components

### 1. **Graph System**
   - **Description**: Manages nodes, edges, and the execution flow of the graph.
   - **Files**:
     - `graph.cpp` / `graph.hpp`: Defines the graph structure, node management, and connectivity between nodes.
     - `node.cpp` / `node.hpp`: Defines individual nodes, their parameters, and logic.
     - `executor.cpp` / `executor.hpp`: Manages the topological sorting and parallel execution of the graph.
     - **Modularization**: Each of these components can be independently built and debugged. Graph execution logic, node processing, and edge connectivity are isolated for easier testing.

### 2. **Graph Execution & Parallelism**
   - **Description**: Executes the graph based on the topological sort of nodes. The nodes are executed in parallel using a thread pool.
   - **Files**:
     - `executor.cpp` / `executor.hpp`: Implements parallel execution with error checking, thread synchronization, and topological sorting.
   - **Key Features**:
     - Topological sorting: Ensures that nodes are processed in dependency order.
     - Parallel execution: Nodes in separate layers are executed concurrently.
   - **Modularization**: The execution logic is standalone, so it can be tested independently, focusing on correct parallel execution and thread management.

### 3. **GUI Integration (Dear ImGui + ImNodes)**
   - **Description**: Provides a node-based graph editor, allowing users to visually design workflows by connecting nodes.
   - **Files**:
     - `gui.cpp` / `gui.hpp`: Implements the graph editor UI using Dear ImGui and ImNodes.
     - **Key Features**:
       - Node-based interface for drag-and-drop functionality.
       - Docking support to organize the workspace and add new windows.
       - Editable parameters for nodes.
   - **Modularization**: The GUI is separate, so the graph’s visual representation and the logic behind node creation, connection, and parameter editing can be debugged independently.

### 4. **Serialization**
   - **Description**: Provides functionality to serialize and deserialize the graph, nodes, and parameters in JSON format.
   - **Files**:
     - `serialization.cpp` / `serialization.hpp`: Implements the functions to save and load project data in JSON format.
   - **Key Features**:
     - `SaveProjectJson`: Saves the current state of the graph, nodes, and edges to a JSON file.
     - `LoadProjectJson`: Loads a project from a JSON file into the system.
   - **Modularization**: Serialization is separated into its own module, making it easy to test and debug independently, focusing on correct saving and loading of graph states.

## Debugging and Error Handling
   - Each module has extensive debugging checks in place to ensure:
     - Errors in node execution, such as invalid parameters or missing nodes, are reported with clear messages.
     - Serialization errors are caught when saving/loading JSON, with descriptive error reporting.
     - The GUI provides feedback for invalid graph states or execution issues.

## Build and Debug Workflow

Each component can be built and debugged independently by using the following structure:

1. **Graph System**: Build and test the core graph functionality (adding nodes, connecting nodes, and executing them).
2. **Execution Logic**: Test the parallel execution with sample graphs to ensure correct topological sorting and thread synchronization.
3. **GUI**: Test the graphical representation and interaction of the graph, ensuring drag-and-drop functionality and parameter editing.
4. **Serialization**: Verify that projects are saved and loaded correctly, ensuring all node states, connections, and parameters are intact.

## CMake Integration
   - The project uses CMake for building and managing dependencies. The following options are available:
     - `WITH_QT` (Qt-based GUI support)
     - `WITH_IMGUI` (ImGui-based GUI support)
     - `WITH_OPENCV`, `WITH_OPEN3D`, `WITH_REALSENSE`, `WITH_PHOTONEO`, `WITH_CREAFORM`, `WITH_ABB`, `WITH_PYTHON` for integration with external libraries.

Each component is built as a separate module, with its own target in CMake, allowing for selective building and debugging.

## Future Work
   - Integration with more node types and external systems (e.g., OpenCV, Open3D, Robotics, etc.).
   - Optimization for large graphs and improved error handling for complex workflows.
   - Further development of modular adapters for various hardware and software interfaces.
