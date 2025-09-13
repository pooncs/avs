# Minimal example used by PyScriptNode

def process(inputs: dict, params: dict) -> dict:
    # Pass-through with a friendly note
    out = dict(inputs)
    entry = params.get('entry', 'process')
    out['note'] = f"Processed in Python entry='{entry}'"
    return out
