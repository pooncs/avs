
# AutoVisionStudio UML (PlantUML)

Files:
- `AutoVisionStudio_packages.puml` — package-level diagram
- `core.puml`, `graph.puml`, `scripting.puml`, `gui.puml`, `adapters.puml`, `nodes.puml`, `runtime.puml`, `app.puml` — module/class diagrams
- `AutoVisionStudio_unified.puml` — convenience file that includes all module diagrams (requires local includes)

## Render
- With Docker: `docker run --rm -v %cd%:/ws plantuml/plantuml -tpng AutoVisionStudio_packages.puml`
- With JAR: `java -jar plantuml.jar -tpng AutoVisionStudio_packages.puml`

## Notes
- Diagrams reflect the scaffold interfaces committed in the provided codebase.
- Extend as modules evolve; keep comments brief.
