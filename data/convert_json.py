#!/usr/bin/env python3
"""
Script to convert JSON spritesheet format from nested structure to simplified format.

Converts from:
{
    "frames": {
        "SpriteName.png": {
            "frame": { "x": 0, "y": 0, "w": 10, "h": 12 },
            ...
        }
    }
}

To:
{
    "frames": {
        "SpriteName.png": {
            "x": 0,
            "y": 0,
            "w": 10,
            "h": 12
        }
    }
}
"""

import json
import sys
import argparse
from pathlib import Path


def convert_json_format(input_data):
    """
    Convert JSON from nested frame format to simplified format.
    
    Args:
        input_data: Dictionary containing the JSON data
        
    Returns:
        Dictionary with converted format
    """
    if "frames" not in input_data:
        raise ValueError("JSON must contain a 'frames' key")
    
    converted = {
        "frames": {}
    }
    
    for frame_name, frame_data in input_data["frames"].items():
        # Extract frame coordinates from nested structure
        if "frame" in frame_data:
            frame_info = frame_data["frame"]
            converted["frames"][frame_name] = {
                "x": frame_info.get("x", 0),
                "y": frame_info.get("y", 0),
                "w": frame_info.get("w", 0),
                "h": frame_info.get("h", 0)
            }
        else:
            # If already in simplified format, copy as-is
            if all(key in frame_data for key in ["x", "y", "w", "h"]):
                converted["frames"][frame_name] = {
                    "x": frame_data.get("x", 0),
                    "y": frame_data.get("y", 0),
                    "w": frame_data.get("w", 0),
                    "h": frame_data.get("h", 0)
                }
            else:
                print(f"Warning: Frame '{frame_name}' doesn't have expected structure, skipping...")
    
    return converted


def main():
    parser = argparse.ArgumentParser(
        description="Convert JSON spritesheet format from nested to simplified structure"
    )
    parser.add_argument(
        "input_file",
        type=str,
        help="Path to input JSON file"
    )
    parser.add_argument(
        "-o", "--output",
        type=str,
        default=None,
        help="Path to output JSON file (default: overwrites input file)"
    )
    parser.add_argument(
        "--indent",
        type=int,
        default=4,
        help="Number of spaces for JSON indentation (default: 4)"
    )
    
    args = parser.parse_args()
    
    # Read input file
    input_path = Path(args.input_file)
    if not input_path.exists():
        print(f"Error: Input file '{input_path}' does not exist", file=sys.stderr)
        sys.exit(1)
    
    try:
        with open(input_path, 'r', encoding='utf-8') as f:
            input_data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in input file: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading input file: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Convert format
    try:
        converted_data = convert_json_format(input_data)
    except Exception as e:
        print(f"Error converting JSON: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Determine output file
    output_path = Path(args.output) if args.output else input_path
    
    # Write output file
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(converted_data, f, indent=args.indent, ensure_ascii=False)
        print(f"Successfully converted JSON: {input_path} -> {output_path}")
        print(f"Converted {len(converted_data['frames'])} frames")
    except Exception as e:
        print(f"Error writing output file: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()

