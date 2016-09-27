/********************************************************************************
* @file --iec101_element.c
* @brief -- 信息元素操作
* @created 2015-9-23
* @history --

********************************************************************************/

#include "iec.h"

#define E_ELMENT        0x10

/**
 * @brief 初始化信息元素体
 * @param e  信息元素
 * @param ident  标识
 */

static void element_base_init(struct info_element *e,enum Element_Ident ident)
{
    e->obj.id=(uint32)e;
    e->obj.obj_type=ident;
}

/**
 * @brief 生成一个信息元素体
 * @param ident  信息元素体标识
 * @return  >0 信息元素   =0 无内存
 */

struct info_element *iec_element_create(enum Element_Ident ident)
{
  struct info_element *elist=(struct info_element *)iec_malloc(sizeof(struct info_element));
  
  iec_memset(elist,0,sizeof(struct info_element));
  
  element_base_init(elist,ident);
  
  switch(ident)
  {
    case SIQ:
      elist->val=ALLOC_ELEMENT(SIQ);
      INIT_ELEMENT(elist->val,SIQ);
      break;
    case DIQ:
      elist->val=ALLOC_ELEMENT(DIQ);
      INIT_ELEMENT(elist->val,DIQ);
      break;
    case QDS:
      elist->val=ALLOC_ELEMENT(QDS);
      INIT_ELEMENT(elist->val,QDS);
      break;
    case QDP:
      elist->val=ALLOC_ELEMENT(QDP);
      INIT_ELEMENT(elist->val,QDP);
      break;
      
    case VTI:
      elist->val=ALLOC_ELEMENT(VTI);
      INIT_ELEMENT(elist->val,VTI);
      break;
    case NVA:
      elist->val=ALLOC_ELEMENT(NVA);
      INIT_ELEMENT(elist->val,NVA);
      break;
    case SVA:
      elist->val=ALLOC_ELEMENT(SVA);
      INIT_ELEMENT(elist->val,SVA);
      break;
    case STD:
      elist->val=ALLOC_ELEMENT(STD);
      INIT_ELEMENT(elist->val,STD);
      break;
      
     case BCR:
      elist->val=ALLOC_ELEMENT(BCR);
      INIT_ELEMENT(elist->val,BCR);
      break;
    case SEP:
      elist->val=ALLOC_ELEMENT(SEP);
      INIT_ELEMENT(elist->val,SEP);
      break;
    case SPE:
      elist->val=ALLOC_ELEMENT(SPE);
      INIT_ELEMENT(elist->val,SPE);
      break;
    case OCI:
      elist->val=ALLOC_ELEMENT(OCI);
      INIT_ELEMENT(elist->val,OCI);
      break;
      
    case BSI:
      elist->val=ALLOC_ELEMENT(BSI);
      INIT_ELEMENT(elist->val,BSI);
      break;
    case FBP:
      elist->val=ALLOC_ELEMENT(FBP);
      INIT_ELEMENT(elist->val,FBP);
      break;
    case SCO:
      elist->val=ALLOC_ELEMENT(SCO);
      INIT_ELEMENT(elist->val,SCO);
      break;
    case DCO:
      elist->val=ALLOC_ELEMENT(DCO);
      INIT_ELEMENT(elist->val,DCO);
      break;
      
    case RCO:
      elist->val=ALLOC_ELEMENT(RCO);
      INIT_ELEMENT(elist->val,RCO);
      break;
    case TM56:
      elist->val=ALLOC_ELEMENT(TM56);
      INIT_ELEMENT(elist->val,TM56);
      break;
    case TM24:
      elist->val=ALLOC_ELEMENT(TM24);
      INIT_ELEMENT(elist->val,TM24);
      break;
    case TM16:
      elist->val=ALLOC_ELEMENT(TM16);
      INIT_ELEMENT(elist->val,TM16);
      break;
      
    case COI:
      elist->val=ALLOC_ELEMENT(COI);
      INIT_ELEMENT(elist->val,COI);
      break;
    case QOI:
      elist->val=ALLOC_ELEMENT(QOI);
      INIT_ELEMENT(elist->val,QOI);
      break;
    case QCC:
      elist->val=ALLOC_ELEMENT(QCC);
      INIT_ELEMENT(elist->val,QCC);
      break;
    case QPM:
      elist->val=ALLOC_ELEMENT(QPM);
      INIT_ELEMENT(elist->val,QPM);
      break;
      
    case QPA:
      elist->val=ALLOC_ELEMENT(QPA);
      INIT_ELEMENT(elist->val,QPA);
      break;
    case QOC:
      elist->val=ALLOC_ELEMENT(QOC);
      INIT_ELEMENT(elist->val,QOC);
      break;
    case QRP:
      elist->val=ALLOC_ELEMENT(QRP);
      INIT_ELEMENT(elist->val,QRP);
      break;
    case FRQ:
      elist->val=ALLOC_ELEMENT(FRQ);
      INIT_ELEMENT(elist->val,FRQ);
      break;
      
    case SRQ:
      elist->val=ALLOC_ELEMENT(SRQ);
      INIT_ELEMENT(elist->val,SRQ);
      break;
    case SCQ:
      elist->val=ALLOC_ELEMENT(SCQ);
      INIT_ELEMENT(elist->val,SCQ);
      break;
    case LSQ:
      elist->val=ALLOC_ELEMENT(LSQ);
      INIT_ELEMENT(elist->val,LSQ);
      break;
    case AFQ:
      elist->val=ALLOC_ELEMENT(AFQ);
      INIT_ELEMENT(elist->val,AFQ);
      break;
      
     case NOF:
      elist->val=ALLOC_ELEMENT(NOF);
      INIT_ELEMENT(elist->val,NOF);
      break;
    case NOS:
      elist->val=ALLOC_ELEMENT(NOS);
      INIT_ELEMENT(elist->val,NOS);
      break;
    case LOF:
      elist->val=ALLOC_ELEMENT(LOF);
      INIT_ELEMENT(elist->val,LOF);
      break;
    case LOS:
      elist->val=ALLOC_ELEMENT(LOS);
      INIT_ELEMENT(elist->val,LOS);
      break;
      
    case CHS:
      elist->val=ALLOC_ELEMENT(CHS);
      INIT_ELEMENT(elist->val,CHS);
      break;
    case SOF:
      elist->val=ALLOC_ELEMENT(SOF);
      INIT_ELEMENT(elist->val,SOF);
      break;
    case QOS:
      elist->val=ALLOC_ELEMENT(QOS);
      INIT_ELEMENT(elist->val,QOS);
      break;
    case SCD:
      elist->val=ALLOC_ELEMENT(SCD);
      INIT_ELEMENT(elist->val,SCD);
      break;
    default:
      break;
  
  }
  
  return elist;
}





