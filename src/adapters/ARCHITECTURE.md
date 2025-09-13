# Thread 6: Open3D Adapter & Point Cloud Nodes

**Modules added**

- `adapters/open3d` – thin pImpl adapter over Open3D point cloud APIs.
- `nodes/pointcloud` – graph nodes using the adapter: voxel downsample, ICP, plane segmentation.

**Build flags**

- Guarded by `WITH_OPEN3D`. When `OFF`, these targets are skipped. citeturn1search1

**Targets**

- `avs_open3d_adapter` (static lib)
- `avs_nodes_pointcloud` (static lib)

**Integration**

In the repo root `CMakeLists.txt`, add:

```cmake
# Options are declared at top-level already per project context
# add_subdirectory only when WITH_OPEN3D is enabled
if(WITH_OPEN3D)
    add_subdirectory(adapters/open3d)
    add_subdirectory(nodes/pointcloud)
endif()
```

Then link nodes library into your app or plugins as needed:

```cmake
target_link_libraries(avs_app PRIVATE avs_nodes_pointcloud)
```

**Node registration**

Nodes call `AVS_REGISTER_NODE(...)` in their headers. If the macro is not available yet, it compiles as a no-op shim, avoiding build breaks; when the real registry is present, the macro will register the node types under the `PointCloud` category. citeturn1search1

**Public API**

The adapter exposes `avs::adapters::open3d::PointCloud` using pImpl, keeping Open3D headers out of public interfaces and downstream recompiles. Algorithms provided:

- `voxelDownsample(voxel)`
- `estimateNormals(radius, max_nn)`
- `icpTo(target, max_corr_dist, max_iterations)` → aligned cloud + 4×4 transform + fitness/RMSE
- `segmentPlane(distance_threshold, ransac_n, num_iterations)` → plane model + inliers/outliers

**Notes**

- C++17, RAII, no raw `new`/`delete`.
- Keep comments brief.
- This thread adds minimal functionality; more nodes can follow (filters, DBSCAN, Poisson meshing, etc.). citeturn1search1
