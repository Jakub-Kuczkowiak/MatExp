//
//// Jakub Kuczkowiak 288587
//
//#include <stdlib.h>
//#include "drzewo.h"
//#include "stack.h"
//
//Wsk_drzewa StworzLisc(char* wyrazenie)
//{
//    Wsk_drzewa nowy;
//
//    if ((nowy = (Wsk_drzewa)malloc(sizeof(struct Wezel))) == NULL) {
//        //fprintf(stderr,"Brak pamieci w stercie dla %lf\n",dana);
//        exit(0);
//    }
//
//    nowy->lewy = NULL;
//    nowy->prawy = NULL;
//    nowy->op = Operators[0];
//    nowy->wyrazenie = wyrazenie;
//
//    return nowy;
//}
//
//Wsk_drzewa StworzWezel(Wsk_drzewa lisc1, Wsk_drzewa lisc2, Operator op)
//{
//    Wsk_drzewa nowy;
//
//    if ((nowy = (Wsk_drzewa)malloc(sizeof(struct Wezel))) == NULL) {
//        //fprintf(stderr,"Brak pamieci w stercie dla %lf\n",dana);
//        exit(0);
//    }
//
//    nowy->lewy = lisc1;
//    nowy->prawy = lisc2;
//    nowy->op = op;
//
//    return nowy;
//}
//
////void WypiszDrzewo(Wsk_drzewa wezel)
////{
////    switch (wezel->op)
////    {
////    case PLUS:
////        WypiszDrzewo(wezel->lewy);
////        printf(" %c ", wezel->op);
////        WypiszDrzewo(wezel->prawy);
////        break;
////
////    case MINUS:
////        WypiszDrzewo(wezel->lewy);
////        printf(" %c ", wezel->op);
////
////        if (wezel->prawy->op == PLUS || wezel->prawy->op == MINUS)
////        {
////            printf("(");
////            WypiszDrzewo(wezel->prawy);
////            printf(")");
////        }
////        else
////        {
////            WypiszDrzewo(wezel->prawy);
////        }
////
////        break;
////
////    case MULTIPLICATION:
////        if (wezel->lewy->op == PLUS || wezel->lewy->op == MINUS)
////        {
////            printf("(");
////            WypiszDrzewo(wezel->lewy);
////            printf(")");
////        }
////        else
////        {
////            WypiszDrzewo(wezel->lewy);
////        }
////
////        printf(" %c ", wezel->op);
////
////        if (wezel->prawy->op == PLUS || wezel->prawy->op == MINUS)
////        {
////            printf("(");
////            WypiszDrzewo(wezel->prawy);
////            printf(")");
////        }
////        else
////        {
////            WypiszDrzewo(wezel->prawy);
////        }
////        break;
////
////    case DIVISION:
////        if (wezel->lewy->op == PLUS || wezel->lewy->op == MINUS)
////        {
////            printf("(");
////            WypiszDrzewo(wezel->lewy);
////            printf(")");
////        }
////        else
////        {
////            WypiszDrzewo(wezel->lewy);
////        }
////
////        printf(" %c ", wezel->op);
////
////        if (wezel->prawy->op != LICZBA)
////        {
////            printf("(");
////            WypiszDrzewo(wezel->prawy);
////            printf(")");
////        }
////        else
////        {
////            WypiszDrzewo(wezel->prawy);
////        }
////        break;
////
////    case LICZBA:
////        printf("%f", wezel->wartosc);
////        break;
////    }
////}
//
//Wsk_drzewa create_tree(char* vector)
//{
//    StackPtr stack;
//    init(&stack);
//
//    for (int i = 0; vector[i] != NULL; i++)
//    {
//        Operator op = GetOperator(vector[i]);
//        if (op.ch != Operators[0].ch)
//        { // this is operator!
//            Wsk_drzewa leaf2 = pop(&stack);
//            Wsk_drzewa leaf1 = pop(&stack);
//
//            Wsk_drzewa wezel = StworzWezel(leaf1, leaf2, op);
//
//            push(&stack, wezel);
//        }
//        else
//        {
//            // this is number or variable
//            char wyrazenie[32];
//            // odczytac wyrazenie!
//            Wsk_drzewa leaf = StworzLisc(wyrazenie);
//
//            push(&stack, leaf);
//        }
//    }
//
//    Wsk_drzewa Korzen = pop(&stack);
//    WypiszDrzewo(Korzen);
//
//    return Korzen;
//}
