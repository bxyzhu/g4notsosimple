#!/usr/bin/env python
"""
    Loops through hdf5 file and prints stuff
"""
import h5py
import numpy as np
import pandas as pd
def main():
    data = HDFData()
    with h5py.File('g4notsosimpleout.hdf5', 'r') as f:
        f.visititems(data)
    dataDict = data.sets
    df = pd.DataFrame.from_dict(data.sets)
    print(df.head(10))
    print(len(df))

class HDFData():
    def __init__(self):
        self.sets = {}
    def __call__(self, name, node):
        if isinstance(node, h5py.Dataset) and isinstance(node[()], np.ndarray):
            self.sets.setdefault(name.split('/')[-2], node[()])
        return None


if __name__ == '__main__':
    main()
