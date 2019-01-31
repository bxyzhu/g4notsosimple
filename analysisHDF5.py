#!/usr/bin/env python
"""
    Loops through hdf5 file and prints stuff
"""
import h5py
import numpy as np
import pandas as pd

def main():
    df = g4todf('g4notsosimpleout.hdf5')
    print(df.head(10))
    print(len(df))


def g4todf(inFile):
    g4data = HDFData()
    with h5py.File(inFile, 'r') as f:
        f.visititems(g4data)
    dataDict = g4data.sets
    df = pd.DataFrame.from_dict(g4data.sets)
    return df


class HDFData():
    def __init__(self):
        self.sets = {}
    def __call__(self, name, node):
        if isinstance(node, h5py.Dataset) and isinstance(node[()], np.ndarray):
            self.sets.setdefault(name.split('/')[-2], node[()])
        return None


if __name__ == '__main__':
    main()
