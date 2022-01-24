ArrayIndex *arrayIndexListAdd(ArrayIndex *ip, int size);
ThreeAddr *threeAddrListAdd(ThreeAddr *tp, NodeType nType, RegIndex result, RegIndex r_opd1, RegIndex r_opd2, Node *n_opd1, Node *n_opd2);
Symbol *symbolListAdd(Symbol *sp, char *varName, int offset, int size, ArrayIndex *ip);
