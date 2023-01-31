#!/usr/bin/env python3

import sys
from os import getcwd
from ntpath import basename
from zipfile import ZipFile
import json

n = 2
partitions_arg = sys.argv[1:]
partitions = final = [partitions_arg[i * n:(i + 1) * n] for i in range((len(partitions_arg) + n - 1) // n )]

with ZipFile('build/openiris.zip', 'w') as archive:
    print('Creating "' + archive.filename + '"', end='\n')
    parts = []

    for [offset, path] in partitions:
        filename = basename(path)
        archive.write(path, filename)
        partition = {
            'path': filename,
            'offset': int(offset, 16),
        }
        parts.append(partition)

    manifest = {
        'chipFamily': 'ESP32',
        'parts': parts,
    }
    archive.writestr('manifest.json', json.dumps(manifest))
