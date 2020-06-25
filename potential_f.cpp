/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2020 Hans Bihs

This file is part of REEF3D.

REEF3D is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
--------------------------------------------------------------------*/

#include"potential_f.h"
#include"solver.h"
#include"ghostcell.h"
#include"fdm.h"
#include"lexer.h"
#include<iomanip>

potential_f::potential_f(lexer* p) 
{
    gcval_pot=49;
}

potential_f::~potential_f()
{
}

void potential_f::start(fdm* a,lexer*p,solver* psolv, ghostcell* pgc)
{
    if(p->mpirank==0 )
	cout<<"starting potential flow solver..."<<endl<<endl;

    starttime=pgc->timer();
	
	pgc->start4(p,a->press,gcval_pot);
	
	LOOP
	a->press(i,j,k) = (p->pos_x()-p->global_xmin)*p->Ui;
	
    ucalc(p,a);
	vcalc(p,a);
	wcalc(p,a);
    
    pgc->start1(p,a->u,10);
	pgc->start2(p,a->v,11);
	pgc->start3(p,a->w,12);

    int itermem=p->N46;
    p->N46=3;
	

    pgc->start4(p,a->press,gcval_pot);

    laplace(p,a);
    rhs(p,a);
    for(int lapliter=0;lapliter<p->I20;++lapliter)
    {
    laplace(p,a);
	psolv->start(p,a,pgc,a->press,a->xvec,a->rhsvec,4,gcval_pot,p->N43);
    pgc->start4(p,a->press,gcval_pot);
    }
	
    ucalc(p,a);
	vcalc(p,a);
	wcalc(p,a);

	pgc->start1(p,a->u,10);
	pgc->start2(p,a->v,11);
	pgc->start3(p,a->w,12);

    endtime=pgc->timer();
    p->laplaceiter=p->solveriter;
	p->laplacetime=endtime-starttime;
	if(p->mpirank==0  && (p->count%p->P12==0))
	cout<<"lapltime: "<<p->laplacetime<<endl;

    p->N46=itermem;
    
   /* LOOP
    a->press(i,j,k) = 0.0;*/
}

void potential_f::ucalc(lexer *p, fdm *a)
{	
	ULOOP
	a->u(i,j,k) = (a->press(i+1,j,k)-a->press(i,j,k))/p->DXP[IP];
	
	if(p->I21==1)
	ULOOP
	if(0.5*(a->phi(i,j,k)+a->phi(i+1,j,k))<-p->F45*p->DXP[IP])
	a->u(i,j,k)=0.0;
    
    if(p->S10==2)
	ULOOP
	if(0.5*(a->topo(i,j,k)+a->topo(i+1,j,k))<-p->F45*p->DXP[IP])
	a->u(i,j,k)=0.0;
}

void potential_f::vcalc(lexer *p, fdm *a)
{	
	VLOOP
	a->v(i,j,k) = (a->press(i,j+1,k)-a->press(i,j,k))/p->DYP[JP];

	if(p->I21==1)
	VLOOP
	if(a->phi(i,j,k)<-p->F45*p->DYP[JP])
	a->v(i,j,k)=0.0;
    
    if(p->S10==2)
	VLOOP
	if(0.5*(a->topo(i,j,k)+a->topo(i,j+1,k))<-p->F45*p->DYP[JP])
	a->v(i,j,k)=0.0;
}

void potential_f::wcalc(lexer *p, fdm *a)
{
	WLOOP
    a->w(i,j,k) = (a->press(i,j,k+1)-a->press(i,j,k))/p->DZP[KP];
	
    if(p->I21==1)
	WLOOP
	if(a->phi(i,j,k)<-p->F45*p->DZP[KP])
	a->w(i,j,k)=0.0;
    
    if(p->S10==2)
	WLOOP
	if(0.5*(a->topo(i,j,k)+a->topo(i,j,k+1))<-p->F45*p->DZP[KP])
	a->w(i,j,k)=0.0;
}

void potential_f::rhs(lexer *p, fdm* a)
{
    count=0;
    LOOP
    {
    a->rhsvec.V[count] = 0.0;
    count++;
    }
}

void potential_f::laplace(lexer *p, fdm *a)
{
	n=0;
    LOOP
	{
	a->M.p[n]  =  1.0/(p->DXP[IP]*p->DXN[IP]) + 1.0/(p->DXP[IM1]*p->DXN[IP])
                + 1.0/(p->DYP[JP]*p->DYN[JP]) + 1.0/(p->DYP[JM1]*p->DYN[JP])
                + 1.0/(p->DZP[KP]*p->DZN[KP]) + 1.0/(p->DZP[KM1]*p->DZN[KP]);

   	a->M.n[n] = -1.0/(p->DXP[IP]*p->DXN[IP]);
	a->M.s[n] = -1.0/(p->DXP[IM1]*p->DXN[IP]);

	a->M.w[n] = -1.0/(p->DYP[JP]*p->DYN[JP]);
	a->M.e[n] = -1.0/(p->DYP[JM1]*p->DYN[JP]);

	a->M.t[n] = -1.0/(p->DZP[KP]*p->DZN[KP]);
	a->M.b[n] = -1.0/(p->DZP[KM1]*p->DZN[KP]);
	
	a->rhsvec.V[n] = 0.0;
	
	++n;
	}
    
    
    n=0;
	LOOP
	{
		if(p->flag4[Im1JK]<0)
		{
		a->rhsvec.V[n] -= a->M.s[n]*a->press(i-1,j,k);
		a->M.s[n] = 0.0;
		}
		
		if(p->flag4[Ip1JK]<0)
		{
		a->rhsvec.V[n] -= a->M.n[n]*a->press(i+1,j,k);
		a->M.n[n] = 0.0;
		}
		
		if(p->flag4[IJm1K]<0)
		{
		a->rhsvec.V[n] -= a->M.e[n]*a->press(i,j-1,k);
		a->M.e[n] = 0.0;
		}
		
		if(p->flag4[IJp1K]<0)
		{
		a->rhsvec.V[n] -= a->M.w[n]*a->press(i,j+1,k);
		a->M.w[n] = 0.0;
		}
		
		if(p->flag4[IJKm1]<0)
		{
		a->rhsvec.V[n] -= a->M.b[n]*a->press(i,j,k-1);
		a->M.b[n] = 0.0;
		}
		
		if(p->flag4[IJKp1]<0)
		{
		a->rhsvec.V[n] -= a->M.t[n]*a->press(i,j,k+1);
		a->M.t[n] = 0.0;
		}

	++n;
	}
}

