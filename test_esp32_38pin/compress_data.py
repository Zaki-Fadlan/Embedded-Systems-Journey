#!/usr/bin/env python3
import os
import gzip
import shutil


def compress_file(source, dest):
    """Compress file using gzip"""
    with open(source, 'rb') as f_in:
        with gzip.open(dest, 'wb') as f_out:
            shutil.copyfileobj(f_in, f_out)
    print(f"Compressed: {source} -> {dest}")


def compress_data_folder():
    """Compress all files in data folder"""
    data_dir = "data"

    if not os.path.exists(data_dir):
        print(f"Data folder not found: {data_dir}")
        return

    for filename in os.listdir(data_dir):
        if filename.endswith(('.html', '.js', '.css')):
            source_path = os.path.join(data_dir, filename)
            dest_path = os.path.join(data_dir, filename + '.gz')

            if os.path.isfile(source_path):
                compress_file(source_path, dest_path)


if __name__ == "__main__":
    compress_data_folder()
    print("Compression complete!")
