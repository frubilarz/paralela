#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
logging.basicConfig(level=logging.DEBUG)

class WeightedQuickUnionUF(object):
    def __init__(self, N, debug=False):
        self.N = N
        self.id = range(N) # Arbol plano
        self.szc = range(N) # Registra el tamaño de las componentes o "clusters", es para el pesado
        self.count = N # Al iniciar el proceso hay N componentes no conexas
        self.debug = debug
        # permite balancear el arbol

    # Se pega un merge entre los componentes que contienen a p y q
    def union(self, p, q):
        if self.debug:
            logging.debug('<WQU>: Conectando nodos {0} y {1}'.format(p, q))
        rootP = self.find(p)
        rootQ = self.find(q)

        if rootP == rootQ: return None # Si las componentes ya estan conexas no hacemos nada

        # Añado el componente mas chico al más grande, balanceando el arbol
        if self.szc[rootP] < self.szc[rootQ]:
            self.id[rootP] = rootQ
            self.szc[rootQ] += self.szc[rootP]
        else:
            self.id[rootQ] = rootP
            self.szc[rootP] += self.szc[rootQ]
        self.count -= 1 # Como hice un merge, dos componentes pasaron a ser una, disminuyo count

    # Sube a traves del arbol, esto da la raiz del componente
    def find(self, p):
        while p != self.id[p]:
            p = self.id[p]
        return p

    # Si ambas componentes estan attacheadas al mismo componentes, estan conectadas
    def connected(self, p, q):
        conectados = self.find(p) == self.find(q)
        if self.debug:
            logging.debug('<WQU>: Checkeando si hay conexión entre {0} y {1}: {2}'.format(p, q, conectados))
        return conectados


# Codigo de pruebas
if __name__ == '__main__':
    qu = WeightedQuickUnionUF(9, debug=True)

    qu.union(0, 3)
    qu.union(2, 4)
    qu.union(5, 8)

    qu.connected(4, 8)
    qu.union(4, 8)
    qu.connected(4, 8)
