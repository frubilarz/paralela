#!/usr/bin/env python
# -*- coding: utf-8 -*-
from quick import WeightedQuickUnionUF
from random import Random
import sys
import numpy
from mpi4py import MPI
import time
#-----------------------------
comm = MPI.COMM_WORLD 
rank = comm.rank
size = comm.size 
N=int(sys.argv[1])
sim = int(sys.argv[2])
t0 = time.time()
#-----------------------------
class PercolationSimulation(object):
    def __init__(self, N, rseed=None):
        self.N = N
        self.qu = WeightedQuickUnionUF(N * N + 2, debug=False) # La grilla es de N * N, pero se agregan dos componentes virtuales
        self.virt_top = N * N
        self.virt_bottom = N * N + 1
        # Usamos un hack: hay dos nodos virtuales en WQU, una para cada borde
        # Conectamos todos los nodos de cada borde a su nodo virtual, luego checkeamos si ambos nodos son conexos
        # Si esto es True, el sistema percola
        for i in range(N):
            self.qu.union(N * N, i) # El nodo N * N es virtual top
        for i in range(N * N - N, N * N):
            self.qu.union(N * N + 1, i) # El nodo N * N + 1 es virtual bottom
        self.open = [False] * (N * N) # Indica si el nodo esta abierto o no
        self.rng = Random(rseed) if rseed else Random()

    def adyacentes(self, p):
        # Retorna los id de los nodos abiertos adyacentes a p
        adyacentes = []
        izq = p - 1
        derecha = p + 1
        arriba = p - self.N
        abajo = p + self.N
        # Checkea a los vecinos del nodo, viendo si realmente son vecinos, y estan abiertos
        for nodo in (izq, derecha, arriba, abajo):
            if 0 < nodo < self.N * self.N and self.open[nodo]:
                adyacentes.append(nodo)
        return adyacentes

    def _percola(self): # Si ambos nodos virtuales son conexos, bingo!
        return self.qu.connected(self.virt_top, self.virt_bottom)

    def umbral(self):
        cerrados = range(self.N * self.N) # Todos los sitios parten cerrados
        self.rng.shuffle(cerrados) # Hacemos un shuffle, para ir abriendo sitios aleatoriamente

        while cerrados:
            nodo = cerrados.pop()
            self.open[nodo] = True # Se abre el nodo
            vecinos = self.adyacentes(nodo) # Se obtienen los nodos adyacentes
            for vecino in vecinos: # Se establece un enlace entre el nodo y cada nodo adyacente
                self.qu.union(nodo, vecino)
            if self._percola(): break # Si el sistema percola, terminamos
        abiertos = float(self.N ** 2 - len(cerrados))
        return abiertos / (self.N * self.N) # La estimación del umbral de percolación

if __name__ == '__main__':
    t0 = time.time()
    prom=0.0
    if rank==0:
        sim1=sim/size+sim%size
    else:
     sim1=sim/size

    for i in range(sim1):
        Perc = PercolationSimulation(N)
        prom += Perc.umbral()
    prom=prom/(sim1)
    if rank!=0:
        comm.send(prom,dest=0)
    if rank==0:
        final=0
        for i in range(1,size):
            final+=comm.recv(source=i)
        final+=prom
        final=final/size
        print "el umbral de percolacion con largo ",N,", ",sim," simulaciones y ",size," procesadores es ",final
        t1 = time.time()
        print "el tiempo de ejecucion es ",t1-t0
