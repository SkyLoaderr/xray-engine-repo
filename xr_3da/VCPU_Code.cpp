#include "stdafx.h"
#define C_EXT extern "C" {
/************************************************************************
-= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
email: ppodsiad@elka.pw.edu.pl
WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:vcpucode.c
Desc:code generation for VCPU
************************************************************************/
C_EXT
/***********************************************************************/
#include "scripting\seer.h"
#include "scripting\internal.h"
#include "scripting\code.h"

#define PUSHAlways_INT	1
#define PUSHAnything	0

NameNPar opcodes_vcpu[]={
	{"NAO",0},//Not arguments opcode
	{"PUSH",1},
	{"DPUSH",1},
	{"POP",1},
	{"NEG",1},//  ???:-1-ARG+1
	{"DNEG",1},
	{"NOT",1},//log.
	{"REP",1},
	{"JMP",1},
	{"CALL",1},
	{"MOV",2},
	{"XCHG",2},
	{"CALLEX",2},
	{"DCALLEX",2},
	{"COPY",2},//see code.h
	//{"CONV",2},//see code.h
	{"ADD",2},
	{"SUB",2},
	{"MUL",2},
	{"DIV",2},
	{"CMPE",2},
	{"CMPG",2},
	{"CMPL",2},
	{"CMPNG",2},
	{"CMPNL",2},
	{"MOD",2},
	{"AND",2},//bit
	{"OR",2},//bit
	{"XOR",2},//bit
	{"ANDL",2},//log.
	{"ORL",2},//logic
	{"SHL",2},
	{"SHR",2},
	{"JTRUE",2},//JTRUE a0,209 // if (a0) 209
	{"JFALSE",2},//31
	{"DADD",2},
	{"DSUB",2},
	{"DMUL",2},
	{"DDIV",2},
	{"DCMPE",2},
	{"DCMPG",2},
	{"DCMPL",2},
	{"DCMPNG",2},
	{"DCMPNL",2},
	{"FIXMUL",2},
	{"FIXDIV",2},
	{"CMOV",2},//copies 1 byte
	{"WMOV",2},//copies 2 bytes
	{"DMOV",2},//copies 8 bytes
	{"IDBL",2},
	{"DINT",2},
	{"FDBL",2},
	{"DFLT",2},
	{"DFIX",2},
	{"FIXDBL",2},
	{"FORK",2},
	{"RET",0},//must be the first of all nonargument
	{"CLI",0},// \Turn OFF\ Multitasking
	{"STI",0},// /     ON /
	{"WAIT",0},// -
	{"ENTER",0},//push bp;mov bp,sp
	{"LEAVE",0},//mov sp,bp; pop bp
	{"NOP",0}
};

int ConvOpcodeTo_Double(int opcode)
{
	switch (opcode)
	{
	case opvcpuMOV:
		opcode=opvcpuDMOV;
		break;
	case opvcpuADD:
		opcode=opvcpuDADD;
		break;
	case opvcpuSUB:
		opcode=opvcpuDSUB;
		break;
	case opvcpuMUL:
		opcode=opvcpuDMUL;
		break;
	case opvcpuDIV:
		opcode=opvcpuDDIV;
		break;
	case opvcpuCMPE:
		opcode=opvcpuDCMPE;
		break;
	case opvcpuCMPG:
		opcode=opvcpuDCMPG;
		break;
	case opvcpuCMPL:
		opcode=opvcpuDCMPL;
		break;
	case opvcpuCMPNG:
		opcode=opvcpuDCMPNG;
		break;
	case opvcpuCMPNL:
		opcode=opvcpuDCMPNL;
		break;
	default:
		serr2(scErr_CodeGen,"Invalid `%s' operand for floating",opcodes_vcpu[opcode].name);
	}
	return opcode;
scError_Exit:
	return 0;
}

int ConvOpcodeTo_Fixed(int opcode)
{
	switch (opcode)
	{
	case opvcpuMOV:
	case opvcpuADD:
	case opvcpuSUB:
	case opvcpuCMPE:
	case opvcpuCMPG:
	case opvcpuCMPL:
	case opvcpuCMPNG:
	case opvcpuCMPNL:
		//Don't change
		break;
	case opvcpuMUL:
		opcode=opvcpuFIXMUL;
		break;
	case opvcpuDIV:
		opcode=opvcpuFIXDIV;
		break;
	default:;
		//       serr(scErr_CodeGen,"Invalid operand for fixed");
	}
	return opcode;
	//scError_Exit:
	//     return 0;
}

void print_value(_value *v)
{
	if (v->adr.flags&adrPOINTER)
	{
		deb0("[");
		if ((v->adr.flags&adrPOINTER)==adrCS) deb0("CS+");
		if ((v->adr.flags&adrPOINTER)==adrDS) deb0("DS+");
		if ((v->adr.flags&adrPOINTER)==adrBP) deb0("BP+");
	}
	if (v->adr.flags&adrREGISTER)
	{
		if (v->adr.address==regCS) {deb0("CS");} else
			if (v->adr.address==regDS) {deb0("DS");} else
				if (v->adr.address==regES) {deb0("ES");} else
					if (v->adr.address==regSS) {deb0("SS");} else
						if (v->adr.address==regIP) {deb0("IP");} else
							if (v->adr.address==regSP) {deb0("SP");} else
								if (v->adr.address==regBP) {deb0("BP");} else
									if (v->adr.address==regCX) {deb0("CX");} else
										deb1("a%d",v->adr.address);
	}
	else {
		if (v->adr.flags&adrPOINTER)//in address
		{deb1("%d",v->adr.address);}
		else
			if (GetValType(v)==valDOUBLE)
			{
				deb1("%f",v->adr.val.dval);
			}
			else
			{
				deb1("%d",v->adr.val.ival);
			}
	}
	if (v->adr.flags&adrPOINTER) deb0("]");
}

void push_value(_value *v,int it)
{
	if ((it==PUSHAnything)&&(GetValType(v)==valDOUBLE)&&(valIsImmidiate(v)))
	{
		push_pmem(&act.code,8,&v->adr.val.dval);deb0(":DBL:");
	}
	else
	{ 
		if (!valIsImmidiate(v))
			pushint_pmem(&act.code,v->adr.address);
		else
			pushint_pmem(&act.code,v->adr.val.ival);
	}
}

void Gen_Align()
{ //align to 4-byte word
	act.code.pos=act.ip;
	if (act.ip%4)//add nops
	{
		int j;
		unsigned char cod=opvcpuNOP;
		for (j=act.ip%4;j<4;j++)
			push_pmem(&act.code,1,&cod);
	}
	act.ip=act.code.pos;
}
/*
Code generation :
-no param opcodes:
00000cod
-1  param opcodes:
codec000 PRtyp000 --par1-- --add---
01234567 - bit positions
-2  param opcodes:
codec-PP RtypRtyp --par1-- --par2--

  type:
  if (!P) decode_type_as typ for _value.flags://numerical value
  &(16+32+64) - 000int 001double 010float 011fixed 100char 101short 110longlong 111(ptr-reserved)
  else//pointer 210-bits    ^^^^^^                                       ^^^^^^^^-2 registers
  valCPU=0,  000
  valDS=4,   001
  valCS=8,   010
  valBP=4+8, 011
  100 ES+       \
  101 SP+
  110 a239+      not used by compiler
  111 a240      /
  WARNING: all operations with params have to be 32bit-Word Aligned
*/


int FixUp_Opcode(int opcode,_value* v1,_value* v2)
{
	if (opcode==-1) return -1;
	
	if (opcodes_vcpu[opcode].pars==0)	return opcode;
	else 
		if (opcodes_vcpu[opcode].pars==1)/////////////////////////////////////
		{
			//DELS pv1=*v1;
			if (GetValType(v1)==valSHORT&&(v1->adr.flags&adrPOINTER))
				valPutToRegister(v1,TRUE);
			if (GetValType(v1)==valCHAR&&(v1->adr.flags&adrPOINTER))
				valPutToRegister(v1,TRUE);
			if (valSizeOf(v1)==8)
			{ 
				switch (opcode)
				{
				case opvcpuPUSH:opcode=opvcpuDPUSH;
					break;
				case opvcpuNEG:opcode=opvcpuDNEG;
					break;
				default:
					serr2(scErr_CodeGen,"Code generation error for %s with double",opcodes_vcpu[opcode].name);
				}
			}
		}
		else
			
			//===============================2 params============
			if(opcodes_vcpu[opcode].pars==2)/////////////////////////////////////
			{
				//DELS pv1=*v1;pv2=*v2;
				if (opcode!=opvcpuDFLT&&
					opcode!=opvcpuFDBL&&
					opcode!=opvcpuIDBL&&
					opcode!=opvcpuFIXDBL&&
					opcode!=opvcpuDFIX&&
					opcode!=opvcpuDINT&&
					opcode!=opvcpuCMOV&&
					opcode!=opvcpuWMOV
					)
				{//Conversion allowed
					if (v1->adr.flags&adrPOINTER)//----------------POINTER-------------
					{
						_value tval;
						switch(GetValType(v1))
						{
						case valCHAR:
							if (opcode!=opvcpuCMOV)
							{
								tval=*v1;
								if(opcode==opvcpuMOV)
								{
									valConvTo_Int(v2);
									return opvcpuCMOV;
								}
								else
								{
									valPutToRegister(v1,TRUE);
									//        valConvTo_Char(v2);
									Gen_Opcode(opcode,v1,v2);
									Gen_Opcode(opvcpuCMOV,&tval,v1);//write-back to mem
									*v2=*v1;*v1=tval;
									return opvcpuCMOV;
								}
								//DELS         *v1=pv1;*v2=pv2;
							}
							break;
						case valSHORT:
							if (opcode!=opvcpuWMOV)
							{
								tval=*v1;
								if(opcode==opvcpuMOV)
								{
									valConvTo_Int(v2);
									return opvcpuWMOV;
								}
								else
								{
									valPutToRegister(v1,TRUE);
									Gen_Opcode(opcode,v1,v2);
									*v2=*v1;*v1=tval;
									return opvcpuWMOV;//write-back to mem
								}
								//DELS         *v1=pv1;*v2=pv2;
							}
							break;
						case valFIXED:
							valConvTo_Fixed(v2);
							return ConvOpcodeTo_Fixed(opcode);
							cerr;
							break;
						case valFLOAT:
							if (opcode!=opvcpuMOV)
							{
								int reg=-1;
								tval=*v1;//also remember where was an original
								if (tval.adr.flags&adrREGISTER)
									reg=tval.adr.address;
								valPutToRegister(v1,FALSE);
								//          if (reg!=-1&&!act.registers[reg].used)
								//             act.registers[reg].used=&tval;//make sure we lock the reg.
								Gen_Opcode(opcode,v1,v2);
								*v2=*v1;*v1=tval;
								return opvcpuMOV;//write-back to mem
							}
							else //valConvTo_Float(v2);
							{if (valIsImmidiate(v2)) valConvTo_Float(v2);
							else
								switch(GetValType(v2))
							{
						case valDOUBLE:
							return opvcpuDFLT;
						case valFIXED:
						case valINT:
						case valSHORT:
						case valCHAR:
							valConvTo_Double(v2);
							return opvcpuDFLT;
						default:;
							}
							}
							break;
						case valDOUBLE:
							valConvTo_Double(v2);
							return ConvOpcodeTo_Double(opcode);
						default:;
						}
					}//pointer - cannot convert variable to different type
					else//------------------------------------------------not a pointer
					{
						int t1,t2=0;
						t1=GetValType(v1);
						t2=GetValType(v2);
						if ((t2==valSHORT)&&(v2->adr.flags&adrPOINTER))//has to be short
						{ 
							if(opcode==opvcpuMOV) opcode=opvcpuWMOV;
							else
							{ valPutToRegister(v2,TRUE);
							}
						}
						if ((t2==valCHAR)&&(v2->adr.flags&adrPOINTER))//has to be char
						{ 
							if(opcode==opvcpuMOV) opcode=opvcpuCMOV;
							else
							{ valPutToRegister(v2,TRUE);
							}
						}
						if (t1==valFIXED)
						{  
							valConvTo_Fixed(v2);t2=valFIXED;
							opcode=ConvOpcodeTo_Fixed(opcode);
							cerr;
						} else
							if (t1==valINT&&t2==valFIXED)
							{  
								valConvTo_Fixed(v1);t1=valFIXED;
								opcode=ConvOpcodeTo_Fixed(opcode);
								cerr;
							}
							
							if ((t2==valFLOAT)&&(v2->adr.flags&adrPOINTER))//has to be float
							{ 
								valPutToRegister(v2,TRUE);
							}
							
							if (floating(valTYPE(t2))||floating(valTYPE(t1)))
							{
								valConvTo_Double(v1);
								valConvTo_Double(v2);
								opcode=ConvOpcodeTo_Double(opcode);
								cerr;
								
								switch(opcode)
								{
								case opvcpuDCMPE:
								case opvcpuDCMPL:
								case opvcpuDCMPG:
								case opvcpuDCMPNL:
								case opvcpuDCMPNG:
									v1->adr.flags=adrREGISTER;
									v1->type.flags=typPREDEFINED;
									v1->type.main=(scSymbol*)(LPVOID(DWORD(valINT)));
									//DELS         pv1.type=v1->type;
									
									//         v1->sym=NULL;
									//         deb1("Now it is of type:%s\n",ValTypeStr(t1));
									break;
								default:;
								}
							}//floating
					}//not a pointer -can modify both
		}//can modify
 		if (opcode==opvcpuJFALSE||opcode==opvcpuJTRUE)
		{
			//TODO:optimize JFALSE0,x->JMP x
			if (valIsImmidiate(v1))
			{
				if (v1->adr.val.ival==0)
				{
					switch(opcode)
					{
					case opvcpuJFALSE:opcode=opvcpuJMP;*v1=*v2;v2=NULL;return opcode;
					case opvcpuJTRUE:return -1;//nothing to do
					}
				}else
				{
					switch(opcode)
					{
					case opvcpuJTRUE:opcode=opvcpuJMP;*v1=*v2;v2=NULL;break;
					case opvcpuJFALSE:return -1;//nothing to do
					}
				}
			}
		}//JTRUE/FALSE optimization
	}//2 params
	return opcode;
scError_Exit:
	return -1;
}

//INSTRUCTION:<0-63>+(64/128)
//DELS
void Gen_Opcode(int opcode,_value* v1,_value* v2)
{
	char cod;
	unsigned char instr[4]={0,0,0,0};
	act.code.pos=act.ip;
	// if (v2) deb1("type2=%s",ValTypeStr(GetValType(v2)));
	opcode=FixUp_Opcode(opcode,v1,v2);
	cerr;
	if (opcode==-1) return;
	if(opcodes_vcpu[opcode].pars==0)
	{
		cod=opcode;
		push_pmem(&act.code,1,&cod);
		act.ip=act.code.pos;
	}
	else
		
		if(opcodes_vcpu[opcode].pars==1)
		{
			Gen_Align();
			print_value(v1);
			cod=opcode;
			instr[0]=cod;
			if (v1->adr.flags&adrPOINTER)//not 000
			{
				instr[1]|=1;//pointer
				if (v1->adr.flags&adrREGISTER)
				{
					instr[1]|=2;//register
					instr[2]=v1->adr.address;//register numer
				}
				instr[1]|=(v1->adr.flags&adrSTOREPOINTER)<<2;//base register
			} else
			{
				if (v1->adr.flags&adrREGISTER)
				{
					instr[1]|=2;//register
					instr[2]=v1->adr.address;//register numer
				}
			}
			
			push_pmem(&act.code,4,instr);
			if (!(instr[1]&2)) // not register - push value
				push_value(v1,PUSHAnything);
			
			act.ip=act.code.pos;
		}
		else
			//===============================2 params============
			if(opcodes_vcpu[opcode].pars==2)
			{
				Gen_Align();
				print_value(v1);
				print_value(v2);
				cod=opcode;
				instr[0]=cod;
				//1-st opcode
				if (v1->adr.flags&adrPOINTER)//not 000
				{
					instr[0]|=64;//pointer
					if (v1->adr.flags&adrREGISTER)
					{
						instr[1]|=1;//register
						instr[2]=v1->adr.address;//register numer
					}
					instr[1]|=((v1->adr.flags&adrSTOREPOINTER))<<1;//base register
				} else
				{
					if (v1->adr.flags&adrREGISTER)
					{
						instr[1]|=1;//register
						instr[2]=v1->adr.address;//register numer
					}
				}
				//2-st opcode
				if (v2->adr.flags&adrPOINTER)//not 000
				{
					instr[0]|=128;//pointer
					if (v2->adr.flags&adrREGISTER)
					{
						instr[1]|=(1<<4);//register
						instr[3]=v2->adr.address;//register numer
					}
					instr[1]|=((v2->adr.flags&adrSTOREPOINTER))<<5;//>>2)<<5;//base register
				} else
				{
					if (v2->adr.flags&adrREGISTER)
					{
						instr[1]|=1<<4;//register
						instr[3]=v2->adr.address;//register numer
					}
				}
				
				push_pmem(&act.code,4,instr);
				if (!(instr[1]&1)) // not register - push value
					push_value(v1,PUSHAnything);
				if (!(instr[1]&(1<<4))) // not register - push value
					push_value(v2,PUSHAnything);
				act.ip=act.code.pos;
				deb0("\n");
			}
			else serr2(scErr_CodeGen, "Code generation error %s","for more than 2 opcodes");
			return;
scError_Exit:
			return;
}

/***********************************************************************/
}; // extern "C"
/***********************************************************************/
