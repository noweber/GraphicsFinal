# -*- coding: utf-8 -*-
"""
Created on Mon Nov  3 13:58:27 2014

@author: bobby
"""

import sys

def readFile(fName):
    verts = []
    norms = []
    faces = []
    texcs = []
    with open(fName) as f:
        for line in f:
            if line[0] == '#' or len(line) < 3:
                continue
            parts = line.split(" ")
            if parts[0] == "v": #vertecies
                verts.append((float(parts[1]),float(parts[2]),float(parts[3])))
            elif parts[0] == "vt": #texture coordinates
                texcs.append((float(parts[1]),float(parts[2])))
            elif parts[0] == "vn": #normals
                norms.append((float(parts[1]),float(parts[2]),float(parts[3])))
            elif parts[0] == "f": #face
                vs = []
                ts = []
                ns = []
                for i in range(1,len(parts)):
                    p2 = parts[i].split("/")
                    vs.append(int(p2[0]))
                    ts.append(int(p2[1]))
                    ns.append(int(p2[2]))
                faces.append((tuple(vs),tuple(ts),tuple(ns)))

    return verts,texcs,norms,faces


if __name__=="__main__":
    if not len(sys.argv) == 3:
        print "Incorrect Usage: python objToModel.py [obj File] [model file]"
        exit()
    oFile = sys.argv[1];
    mFile = sys.argv[2];
    vs,ts,ns,fs = readFile(oFile)
    
    with open(mFile,'w') as modFile:
        modFile.write(str(len(fs)*8*3)+"\n")
        for i in range(len(fs)):
            #pos,uv,norm
            f = fs[i]
            for k in range(3):
                v = vs[f[0][k]-1]
                t = ts[f[1][k]-1]
                n = ns[f[2][k]-1]
                modFile.write(str(v[0])+"\n")
                modFile.write(str(v[1])+"\n")
                modFile.write(str(v[2])+"\n")
                modFile.write(str(t[0])+"\n")
                modFile.write(str(1-t[1])+"\n")
                modFile.write(str(n[0])+"\n")
                modFile.write(str(n[1])+"\n")
                modFile.write(str(n[2])+"\n")
        