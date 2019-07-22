/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2019 Hans Bihs

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

#include"kepsilon.h"
#include"lexer.h"
#include"fdm.h"
#include"ghostcell.h"
#include"vrans_f.h"
#include"vrans_v.h"

kepsilon::kepsilon(lexer* p, fdm* a, ghostcell *pgc) : rans_io(p,a), bc_kepsilon(p)
{
    if(p->B269==0)
	pvrans = new vrans_v(p,a,pgc);
	
	if(p->B269==1)
	pvrans = new vrans_f(p,a,pgc);
}

kepsilon::~kepsilon()
{
}

void kepsilon::clearfield(lexer *p, fdm*  a, field& b)
{
	LOOP
	b(i,j,k)=0.0;
}

void kepsilon::isource(lexer *p, fdm* a)
{
	ULOOP
	a->F(i,j,k)=0.0;
}

void kepsilon::jsource(lexer *p, fdm* a)
{
	VLOOP
	a->G(i,j,k)=0.0;
}

void kepsilon::ksource(lexer *p, fdm* a)
{
	WLOOP
	a->H(i,j,k)=0.0;
}

void kepsilon::eddyvisc(fdm*  a,lexer* p, field& kk,field& eps, ghostcell* pgc)
{
	double H;
	double epsi = 1.6*p->dx;
	double factor;
	
	if(p->T30==0)
	LOOP
	a->eddyv(i,j,k) = p->cmu*kk(i,j,k)*kk(i,j,k)/eps(i,j,k);

    if(p->T30==1)
	LOOP
    {
	a->eddyv(i,j,k) = MAX(p->cmu*MAX(kk(i,j,k)*kk(i,j,k)
					  /((eps(i,j,k))>(1.0e-20)?(eps(i,j,k)):(1.0e20)),0.0),
					  0.0001*a->visc(i,j,k));
	}

    if(p->T30==2)
	LOOP
    {
		if(a->phi(i,j,k)>epsi)
		H=1.0;

		if(a->phi(i,j,k)<-epsi)
		H=0.0;

		if(fabs(a->phi(i,j,k))<=epsi)
		H=0.5*(1.0 + a->phi(i,j,k)/epsi + (1.0/PI)*sin((PI*a->phi(i,j,k))/epsi));
		
		factor = H*p->T31 + (1.0-H)*p->T32;
		
	a->eddyv(i,j,k) = MAX(MIN(p->cmu*MAX(kk(i,j,k)*kk(i,j,k)
					  /((eps(i,j,k))>(1.0e-20)?(eps(i,j,k)):(1.0e20)),0.0),fabs(factor*kin(i,j,k))/strainterm(p,a)),
					  0.0001*a->visc(i,j,k));
	}
	
	if(p->T10==21)
	LOOP
	a->eddyv(i,j,k) = MIN(a->eddyv(i,j,k), p->dx*p->cmu*pow((kin(i,j,k)>(1.0e-20)?(kin(i,j,k)):(1.0e20)),0.5));

    pvrans->eddyv_func(p,a);    

	pgc->start4(p,a->eddyv,29);
}

void  kepsilon::kinsource(lexer *p,fdm* a,field& kk,field& eps)
{
    if(p->T40==0)
    LOOP
    {
	if(wallf(i,j,k)==0)
	a->L(i,j,k) = pk(p,a) - eps(i,j,k);

    a->maxK=MAX(fabs(a->L(i,j,k)),a->maxK);
    }


    if(p->T40==1)
	LOOP
	{
	if(wallf(i,j,k)==0)
	a->L(i,j,k) = pk(p,a) - MAX(eps(i,j,k),0.0);
	
    a->maxK=MAX(fabs(a->L(i,j,k)),a->maxK);
	}


    if(p->T40==2 || p->T40==3)
	LOOP
	{
	if(wallf(i,j,k)==0)
	a->L(i,j,k) = MIN(pk(p,a), p->T42*p->cmu*MAX(kin(i,j,k),0.0)*MAX(eps(i,j,k),0.0))
                  -   MAX(eps(i,j,k),0.0);

    a->maxK=MAX(fabs(a->L(i,j,k)),a->maxK);
	}
    
    pvrans->ke_source(p,a,kin);
}

void  kepsilon::epssource(lexer *p,fdm* a,field& kk,field& eps)
{
	double epsi = 1.6*p->dx;
	double dirac,dn;

    if(p->T40==0)
	LOOP
	{
	a->L(i,j,k) = ke_c_1e * (eps(i,j,k)/kk(i,j,k))*pk(p,a)

				- ke_c_2e * eps(i,j,k)*eps(i,j,k)/kk(i,j,k);

    a->maxE=MAX(fabs(a->L(i,j,k)),a->maxE);
	}


    if(p->T40==1 || p->T40==2)
	LOOP
	{
	a->L(i,j,k) = ke_c_1e * (eps(i,j,k)/(fabs(kk(i,j,k))>(1.0e-10)?(fabs(kk(i,j,k))):(1.0e20)))*pk(p,a)

				- ke_c_2e * eps(i,j,k)*MAX((eps(i,j,k))/(fabs(kk(i,j,k))>(1.0e-10)?(fabs(kk(i,j,k))):(1.0e20)),0.0);

    a->maxE=MAX(fabs(a->L(i,j,k)),a->maxE);
	}

    if(p->T40==3)
	LOOP
	{
	a->L(i,j,k) = ke_c_1e * MIN(pk(p,a), p->T42*p->cmu*MAX(eps(i,j,k),0.0))

				- ke_c_2e * eps(i,j,k)*MAX((eps(i,j,k))/(fabs(kk(i,j,k))>(1.0e-10)?(fabs(kk(i,j,k))):(1.0e20)),0.0);

    a->maxE=MAX(fabs(a->L(i,j,k)),a->maxE);
	}
	
    pvrans->eps_source(p,a,kin,eps);
}

void kepsilon::epsfsf(lexer *p, fdm* a,ghostcell *pgc,field& kk,field& eps)
{
	double epsi = p->T38*p->dx;
	double dirac;
	
	
	if(p->T36>0)
	LOOP
	{
		if(fabs(a->phi(i,j,k))<epsi)
		dirac = (0.5/epsi)*(1.0 + cos((p->T39*PI*a->phi(i,j,k))/epsi));
		
		if(fabs(a->phi(i,j,k))>=epsi)
		dirac=0.0;
	
	if(dirac>0.0 && p->T36==1)
	eps(i,j,k) = dirac*2.5*pow(p->cmu,0.75)*pow(fabs(kin(i,j,k)),1.5)*(1.0/p->T37);
	
	if(dirac>0.0 && p->T36==2)
	eps(i,j,k) = dirac*2.5*pow(p->cmu,0.75)*pow(fabs(kin(i,j,k)),1.5)*(1.0/p->T37 + 1.0/(a->walld(i,j,k)>1.0e-20?a->walld(i,j,k):1.0e20));
	}
}